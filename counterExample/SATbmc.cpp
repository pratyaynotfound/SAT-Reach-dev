#include <counterExample/SATbmc.h>

SATbmc ::SATbmc(const std::list<hybrid_automata::ptr> &haPtr, const std::list<initial_state::ptr> &init, const forbidden_states &forbidden,
                ReachabilityParameters &r_params, const userOptions &user_ops) : ha_ptrs(haPtr), init(init), forbidden_s(forbidden),
                                                                                 reach_params(r_params), user_ops(user_ops), c(), ha_encoding(c), S(c)
{
    this->k = user_ops.get_bfs_level();
}

void SATbmc ::mk_constrains(unsigned int k1, hybrid_automata::ptr ha_ptr, int haid)
{
    location::ptr source_ptr = ha_ptr->getInitialLocation();
    int u = source_ptr->getLocId();

    // initial clause(only for 1 initial location for now due to Hyst does not support multiple initial location. thats why we only add one location.)
    std::vector<unsigned int> init_locs;
    init_locs.push_back(u);

    z3::expr_vector init_trans_expr(c);

    for (auto loc_id : init_locs)
    {
        z3::expr_vector exp1(c);
        auto neighbor_nodes = source_ptr->getOutGoingTransitions();
        for (auto trans_id = neighbor_nodes.begin(); trans_id != neighbor_nodes.end(); trans_id++)
        {
            transition::ptr trans_ptr = (*trans_id);
            std::string trans_name;
            z3::expr trans = c.bool_const((trans_ptr->getLabel() + "_1").c_str());
            exp1.push_back(trans);
        }
        z3::expr sttr = c.bool_const(("Stutter_" + std::to_string(loc_id) + "_" + std::to_string(haid) + "_1").c_str());
        exp1.push_back(sttr);
        init_trans_expr.push_back(mk_or(exp1));
    }
    this->S.add(mk_or(init_trans_expr));
    // init condition end

    // EXCLUDE Transitions
    for (unsigned int i = 1; i <= k1; i++)
    {
        auto list_locations = ha_ptr->getAllLocations();

        // labeled trans excel
        for (auto loc_select = list_locations.begin(); loc_select != list_locations.end(); loc_select++)
        {
            auto neighbors = loc_select->second->getOutGoingTransitions();
            for (auto trans = neighbors.begin(); trans != neighbors.end(); trans++)
            {
                z3::expr_vector excel(c);
                z3::expr trans_id_curr = c.bool_const(((*trans)->getLabel() + "_" + std::to_string(i)).c_str());

                // for normal transitions
                for (auto loc_it = list_locations.begin(); loc_it != list_locations.end(); loc_it++)
                {
                    auto neighbors_it = loc_it->second->getOutGoingTransitions();
                    for (auto trans_2 = neighbors_it.begin(); trans_2 != neighbors_it.end(); trans_2++)
                    {
                        if ((*trans) != (*trans_2))
                        {
                            z3::expr trans_id_ex = c.bool_const(((*trans_2)->getLabel() + "_" + std::to_string(i)).c_str());
                            excel.push_back(!trans_id_ex);
                        }
                    }
                }
                // for stutter
                for (auto sttr_it = list_locations.begin(); sttr_it != list_locations.end(); sttr_it++)
                {
                    z3::expr trans_id_ex_sttr = c.bool_const(("Stutter_" + std::to_string(sttr_it->first) + "_" + std::to_string(haid) + "_" + std::to_string(i)).c_str());
                    excel.push_back(!trans_id_ex_sttr);
                }
                z3::expr trans_excel = implies(trans_id_curr, (mk_and(excel)));
                this->S.add(trans_excel);
            }

            // stutter exclude
            z3::expr_vector excel_sttr(c);
            z3::expr trans_id_curr = c.bool_const(("Stutter_" + std::to_string(loc_select->first) + "_" + std::to_string(haid) + "_" + std::to_string(i)).c_str());
            // for normal transitions
            for (auto loc_it = list_locations.begin(); loc_it != list_locations.end(); loc_it++)
            {
                auto neighbors_it = loc_it->second->getOutGoingTransitions();
                for (auto trans_2 = neighbors_it.begin(); trans_2 != neighbors_it.end(); trans_2++)
                {
                    z3::expr trans_id_ex = c.bool_const(((*trans_2)->getLabel() + "_" + std::to_string(i)).c_str());
                    excel_sttr.push_back(!trans_id_ex);
                }
            }
            // for stutter
            for (auto sttr_it = list_locations.begin(); sttr_it != list_locations.end(); sttr_it++)
            {
                if (loc_select->first != sttr_it->first)
                {
                    z3::expr trans_id_ex_sttr = c.bool_const(("Stutter_" + std::to_string(sttr_it->first) + "_" + std::to_string(haid) + "_" + std::to_string(i)).c_str());
                    excel_sttr.push_back(!trans_id_ex_sttr);
                }
            }
            z3::expr trans_excel = implies(trans_id_curr, (mk_and(excel_sttr)));
            this->S.add(trans_excel);
        }
    } // Exclude trans end

    // movement
    auto list_locations = ha_ptr->getAllLocations();
    for (unsigned int i = 1; i < k1; i++)
    {
        for (map<int, location::ptr>::iterator loc = list_locations.begin(); loc != list_locations.end(); loc++)
        {

            // labeled transitions
            list<transition::ptr> trans = loc->second->getOutGoingTransitions();

            for (list<transition::ptr>::iterator trans_id = trans.begin(); trans_id != trans.end(); trans_id++)
            {
                location::const_ptr neighbor_loc = ha_ptr->getLocation((*trans_id)->getDestinationLocationId());
                list<transition::ptr> neighbor_trans = neighbor_loc->getOutGoingTransitions();
                z3::expr curr_trans = c.bool_const(((*trans_id)->getLabel() + "_" + std::to_string(i)).c_str());
                z3::expr_vector next_trans(c);
                // for normal locations
                for (list<transition::ptr>::iterator n_trans_id = neighbor_trans.begin(); n_trans_id != neighbor_trans.end(); n_trans_id++)
                {
                    if (trans_id != n_trans_id)
                    {
                        z3::expr next_trans_expr = c.bool_const(((*n_trans_id)->getLabel() + "_" + std::to_string(i + 1)).c_str());
                        next_trans.push_back(next_trans_expr);
                    }
                }
                // for stutters
                z3::expr next_trans_expr_sttr = c.bool_const(("Stutter_" + std::to_string((*trans_id)->getDestinationLocationId()) + "_" + std::to_string(haid) + "_" + std::to_string(i + 1)).c_str());
                next_trans.push_back(next_trans_expr_sttr);

                z3::expr movement_cl = implies(curr_trans, mk_or(next_trans));
                this->S.add(movement_cl);
            }

            // stutter transitions
            z3::expr curr_trans_sttr = c.bool_const(("Stutter_" + std::to_string(loc->first) + "_" + std::to_string(haid) + "_" + std::to_string(i)).c_str());
            z3::expr_vector next_sttr_trans(c);
            next_sttr_trans.push_back(c.bool_const(("Stutter_" + std::to_string(loc->first) + "_" + std::to_string(haid) + "_" + std::to_string(i + 1)).c_str()));
            for (auto sttr_trans_it = trans.begin(); sttr_trans_it != trans.end(); sttr_trans_it++)
            {
                z3::expr next_trans_expr = c.bool_const(((*sttr_trans_it)->getLabel() + "_" + std::to_string(i + 1)).c_str());
                next_sttr_trans.push_back(next_trans_expr);
            }
            z3::expr movement_sttr = implies(curr_trans_sttr, mk_or(next_sttr_trans));
            this->S.add(movement_sttr);
        }
    }

    // // Destination
    z3::expr_vector dest_cl(c);
    for (auto locs_it = list_locations.begin(); locs_it != list_locations.end(); locs_it++)
    {
        auto neighbors = locs_it->second->getOutGoingTransitions();
        for (auto out_trans_it = neighbors.begin(); out_trans_it != neighbors.end(); out_trans_it++)
        {
            if ((*out_trans_it)->getDestinationLocationId() == (forbidden_s).at(haid - 1).first)
            {
                dest_cl.push_back(c.bool_const(((*out_trans_it)->getLabel() + "_" + std::to_string(k1)).c_str()));
            }
        }
        if (locs_it->first == (forbidden_s).at(haid - 1).first)
        {
            dest_cl.push_back(c.bool_const(("Stutter_" + std::to_string(locs_it->first) + "_" + std::to_string(haid) + "_" + std::to_string(k1)).c_str()));
        }
    }
    this->S.add(mk_or(dest_cl));
}

void SATbmc::make_constraints(int k1)
{
    int i = 1;
    for (auto ha = ha_ptrs.begin(); ha != ha_ptrs.end(); ha++, i++)
    {
        mk_constrains(k1, (*ha), i);
    }
}

Path SATbmc::get_nextPath(unsigned int k1)
{
    Path p;
    if (S.check() == z3::sat)
    {
        z3::model m = this->S.get_model();
        int it = 1;

        for (auto ha_it = ha_ptrs.begin(); ha_it != ha_ptrs.end(); ha_it++, it++)
        {
            auto list_locations = (*ha_it)->getAllLocations();
            std::vector<std::string> p_seg;
            for (unsigned int i = 0; i <= k1; i++)
            {
                for (auto locs_it = list_locations.begin(); locs_it != list_locations.end(); locs_it++)
                {

                    std::string sttr_trns_str = "Stutter_" + std::to_string(locs_it->first) + "_" + std::to_string(it) + "_" + std::to_string(i);
                    z3::expr check_trans_sttr = c.bool_const((sttr_trns_str.c_str()));

                    if (m.eval(check_trans_sttr).is_true())
                    {
                        p_seg.push_back(std::to_string(locs_it->first).c_str());
                        p_seg.push_back(sttr_trns_str);
                        if(i==k1){
                            p_seg.push_back(std::to_string(locs_it->first).c_str());
                        }
                    }

                    auto neighbors = locs_it->second->getOutGoingTransitions();
                    for (auto trans_it = neighbors.begin(); trans_it != neighbors.end(); trans_it++)
                    {
                        std::string trans_str = (*trans_it)->getLabel() + "_" + std::to_string(i);
                        z3::expr check_trans = c.bool_const((trans_str).c_str());
                        if (m.eval(check_trans).is_true())
                        {
                            p_seg.push_back(std::to_string(locs_it->first).c_str());
                            p_seg.push_back(trans_str);
                            if (i == k1)
                            {
                                p_seg.push_back(std::to_string((*trans_it)->getDestinationLocationId()).c_str());
                            }
                        }
                    }
                }
            }
            p.push_back(p_seg);
        }
    }
    return p;
}

void SATbmc::print_path(Path p)
{
    for (auto path_it_out : p)
    {
        for (auto path_it_in : path_it_out)
        {
            std::cout << path_it_in << " -> ";
        }
        std::cout << std::endl;
    }
}

void SATbmc::_negation(Path P)
{
    z3::expr_vector neg_trans(c);
    for (auto path_it_out : P)
    {
        int count = 1;
        for (auto path_it_in : path_it_out)
        {
            if (count % 2 == 0)
            {
                neg_trans.push_back(!(c.bool_const((path_it_in).c_str())));
            }
            count++;
        }
    }
    this->S.add(mk_or(neg_trans));
}

unsigned int SATbmc::safe()
{

    for (unsigned int k1 = 1; k1 <= k; k1++)
    {
        make_constraints(k1);
        int i = 1;

        // std::cout<<S;
        Path P = get_nextPath(k1);
        int count = 0;
        while (P.size() != 0)
        {
            std::cout << "safe, depth:" << k1 << "\tpath no:" << ++count << std::endl;
            // print_path(P);
            _negation(P);
            P = get_nextPath(k1);
        }
        S.reset();
        // if (S.check() == z3::unsat)
        // {
        //     std::cout << "Unsat" << std::endl;
        //     // std::cout << S << std::endl; //debug
        //     S.reset();
        // }
    }

    // std::cout << "List of HA object found!!!!" << std::endl;

    // for (std::list<hybrid_automata::ptr>::const_iterator it_ha_comp = ha_ptrs.begin(); it_ha_comp != ha_ptrs.end(); it_ha_comp++)
    // {
    //     std::cout << "Dimension of HA: " << it_ha_comp->get()->getDimension() << std::endl;
    //     it_ha_comp->get()->print_var_index_map();
    // }

    return 1;
}