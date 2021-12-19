/*	DESKGSX.C	06/01/84 - 02/02/85	Lee Lorenzen	*/
/*	merge source	5/27/87			mdf		*/

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Desktop					  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1985			Digital Research Inc.
*	-------------------------------------------------------------
*/

/*
*	Calls used in DESKTOP:
*
*	vsf_interior();
*	vr_recfl();
*	vst_height(); 
*	vsl_type();
*	vsl_udsty();
*	vsl_width();
*	v_pline();
*	vst_clip();
*	vr_cpyfm();
*	vq_extnd();
*	v_clsvwk( handle )
*	v_opnvwk( pwork_in, phandle, pwork_out )
*/


#include	"ppddesk.h"



VOID __near gsx_vclose()
{
	v_clsvwk(gl_handle);
}


VOID __near gsx_vopen()
{
	WORD		i;
	WORD		intin[11];

	for(i=0; i<10; i++) intin[i] = 1;
	intin[10] = 2;			/* device coordinate space */

	v_opnvwk( intin, &gl_handle, gl_workout );
}



VOID __near gsx_mon() 
{
	graf_mouse(M_ON, 0x0L);
}


VOID __near gsx_moff() 
{
	graf_mouse(M_OFF, 0x0L);
}


