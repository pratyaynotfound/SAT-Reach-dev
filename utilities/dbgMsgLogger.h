/*
 * dbg_msg_logger.h
 *
 *  Created on: 24-Jul-2019
 *      Author: rajarshi
 */

#ifndef XSPEED_UTILITIES_DBG_MSG_LOGGER_H_
#define XSPEED_UTILITIES_DBG_MSG_LOGGER_H_


#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

#endif /* XSPEED_UTILITIES_DBG_MSG_LOGGER_H_ */
