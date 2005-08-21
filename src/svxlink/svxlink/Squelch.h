/**
@file	 Squelch.h
@brief   Base class for implementing a squelch detector
@author  Tobias Blomberg / SM0SVX
@date	 2005-08-02

This file contains the base class for implementing a squelch detector

\verbatim
SvxLink - A Multi Purpose Voice Services System for Ham Radio Use
Copyright (C) 2004-2005  Tobias Blomberg / SM0SVX

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
\endverbatim
*/

/** @example Squelch_demo.cpp
An example of how to use the Squelch class
*/


#ifndef SQUELCH_INCLUDED
#define SQUELCH_INCLUDED


/****************************************************************************
 *
 * System Includes
 *
 ****************************************************************************/

#include <sigc++/signal_system.h>
#include <string>
#include <iostream>


/****************************************************************************
 *
 * Project Includes
 *
 ****************************************************************************/

#include <AsyncConfig.h>


/****************************************************************************
 *
 * Local Includes
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Forward declarations
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Namespace
 *
 ****************************************************************************/

//namespace MyNameSpace
//{


/****************************************************************************
 *
 * Forward declarations of classes inside of the declared namespace
 *
 ****************************************************************************/

  

/****************************************************************************
 *
 * Defines & typedefs
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Exported Global Variables
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Class definitions
 *
 ****************************************************************************/

/**
@brief	The base class for implementing a squelch detector
@author Tobias Blomberg / SM0SVX
@date   2005-08-02

This class is used as the base when implementing a squelch detector. The
detector should implement the \em processSamples function. In that function,
call \em setOpen to indicate if the squelch is opened or closed.

\include Squelch_demo.cpp
*/
class Squelch : public SigC::Object
{
  public:
    /**
     * @brief 	Default constuctor
     */
    explicit Squelch(void)
      : m_open(false), hangtime(1), hangtime_left(0) {}
  
  
    /**
     * @brief 	Destructor
     */
    virtual ~Squelch(void) {}
  
    virtual bool initialize(Async::Config& cfg, const std::string& rx_name)
    {
      int sql_hangtime = 0;
      std::string value;
      if (cfg.getValue(rx_name, "SQL_HANGTIME", value))
      {
      	sql_hangtime = atoi(value.c_str())*8;
      }
      setHangtime(sql_hangtime);
      
      return true;
    }
  
    /**
     * @brief 	Set the time the squelch should hang open after squelch close
     * @param 	hang_samples The number of samples to hang
     */
    void setHangtime(int hang_samples)
    {
      this->hangtime = (hang_samples >= 1) ? hang_samples : 1;
    }
    
    /**
     * @brief 	Reset the squelch detector
     *
     *  Reset the squelch so that the detection process starts from
     *	the beginning again.
     */
    virtual void reset(void) {}

    /**
     * @brief 	Handle incoming audio
     * @param 	samples A buffer containing samples
     * @param 	count The number of samples in the buffer
     * @return	Return the number of processed samples
     */
    int audioIn(short *samples, int count)
    {
      int ret_count = processSamples(samples, count);
      if (hangtime_left > 0)
      {
      	hangtime_left -= count;
	if (hangtime_left <= 0)
	{
	  m_open = false;
	  squelchOpen(false);
	}
      }
      return ret_count;
    }
    
    /**
     * @brief 	Get the state of the squelch
     * @return	Return \em true if the squelch is open, or else \em false
     */
    bool isOpen(void) const { return m_open || (hangtime_left > 0); }
    
    /**
     * @brief 	A signal that indicates when the squelch state changes
     * @param 	is_open Is set to \em true if the squelch is open or else
     *	      	\em false
     */
    SigC::Signal1<void, bool> 	      squelchOpen;
    
    
  protected:
    /**
     * @brief 	Process the incoming samples in the squelch detector
     * @param 	samples A buffer containing samples
     * @param 	count The number of samples in the buffer
     * @return	Return the number of processed samples
     */
    virtual int processSamples(short *samples, int count) = 0;

    /**
     * @brief 	Set the state of the squelch
     * @param 	is_open Set to \em true if the squelch is open or else \em false
     */
    void setOpen(bool is_open)
    {
      if (is_open)
      {
	hangtime_left = 0;
      	if (!m_open)
      	{
      	  m_open = true;
	  squelchOpen(true);
      	}
      }
      else if (m_open && (hangtime_left <= 0))
      {
	hangtime_left = hangtime;
      }
    }
    
    
  private:
    std::string   m_name;
    bool      	  m_open;
    int       	  hangtime;
    int       	  hangtime_left;

    Squelch(const Squelch&);
    Squelch& operator=(const Squelch&);
    
};  /* class Squelch */


//} /* namespace */

#endif /* SQUELCH_INCLUDED */



/*
 * This file has not been truncated
 */
