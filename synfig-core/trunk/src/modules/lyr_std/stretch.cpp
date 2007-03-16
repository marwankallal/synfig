/* === S Y N F I G ========================================================= */
/*!	\file stretch.cpp
**	\brief Template Header
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#define SYNFIG_NO_ANGLE

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "stretch.h"
#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>
#include <synfig/transform.h>

#endif

/* === U S I N G =========================================================== */

using namespace etl;
using namespace std;
using namespace synfig;

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(Layer_Stretch);
SYNFIG_LAYER_SET_NAME(Layer_Stretch,"stretch");
SYNFIG_LAYER_SET_LOCAL_NAME(Layer_Stretch,_("Stretch"));
SYNFIG_LAYER_SET_CATEGORY(Layer_Stretch,_("Distortions"));
SYNFIG_LAYER_SET_VERSION(Layer_Stretch,"0.1");
SYNFIG_LAYER_SET_CVS_ID(Layer_Stretch,"$Id$");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */

Layer_Stretch::Layer_Stretch():
	amount(1,1),
	center(0,0)
{
}


bool
Layer_Stretch::set_param(const String & param, const ValueBase &value)
{
	IMPORT(amount);
	IMPORT(center);

	return false;
}

ValueBase
Layer_Stretch::get_param(const String &param)const
{
	EXPORT(amount);
	EXPORT(center);

	EXPORT_NAME();
	EXPORT_VERSION();

	return ValueBase();
}

Layer::Vocab
Layer_Stretch::get_param_vocab()const
{
	Layer::Vocab ret;

	ret.push_back(ParamDesc("amount")
		.set_local_name(_("Amount"))
	);

	ret.push_back(ParamDesc("center")
		.set_local_name(_("Center"))
	);

	return ret;
}

synfig::Layer::Handle
Layer_Stretch::hit_check(synfig::Context context, const synfig::Point &pos)const
{
	Point npos(pos);
	npos[0]=(npos[0]-center[0])/amount[0]+center[0];
	npos[1]=(npos[1]-center[1])/amount[1]+center[1];
	return context.hit_check(npos);
}

Color
Layer_Stretch::get_color(Context context, const Point &pos)const
{
	Point npos(pos);
	npos[0]=(npos[0]-center[0])/amount[0]+center[0];
	npos[1]=(npos[1]-center[1])/amount[1]+center[1];
	return context.get_color(npos);
}

class Stretch_Trans : public Transform
{
	etl::handle<const Layer_Stretch> layer;
public:
	Stretch_Trans(const Layer_Stretch* x):Transform(x->get_guid()),layer(x) { }

	synfig::Vector perform(const synfig::Vector& x)const
	{
		return Vector((x[0]-layer->center[0])*layer->amount[0]+layer->center[0],(x[1]-layer->center[1])*layer->amount[1]+layer->center[1]);
	}

	synfig::Vector unperform(const synfig::Vector& x)const
	{
		return Vector((x[0]-layer->center[0])/layer->amount[0]+layer->center[0],(x[1]-layer->center[1])/layer->amount[1]+layer->center[1]);
	}
};
etl::handle<Transform>
Layer_Stretch::get_transform()const
{
	return new Stretch_Trans(this);
}

bool
Layer_Stretch::accelerated_render(Context context,Surface *surface,int quality, const RendDesc &renddesc, ProgressCallback *cb)const
{
	RendDesc desc(renddesc);
	desc.clear_flags();
    // Adjust the top_left and bottom_right points
	// for our zoom amount
	Point npos;
	npos[0]=(desc.get_tl()[0]-center[0])/amount[0]+center[0];
	npos[1]=(desc.get_tl()[1]-center[1])/amount[1]+center[1];
	desc.set_tl(npos);
	npos[0]=(desc.get_br()[0]-center[0])/amount[0]+center[0];
	npos[1]=(desc.get_br()[1]-center[1])/amount[1]+center[1];
	desc.set_br(npos);

	// Render the scene
	return context.accelerated_render(surface,quality,desc,cb);
}
