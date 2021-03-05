/*
 * Copyright (C) 2021 Hermann Meyer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */

#pragma once

#ifndef SRC_HEADERS_GXEFFECTS_H_
#define SRC_HEADERS_GXEFFECTS_H_

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include "lv2/lv2plug.in/ns/ext/atom/forge.h"
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#define PLUGIN_URI "https://github.com/brummer10/XWaveView.lv2"
#define PLUGIN_UI_URI "https://github.com/brummer10/XWaveView.lv2_gui"


typedef enum
{
    EFFECTS_OUTPUT,
    EFFECTS_INPUT,
    NOTIFY,
} PortIndex;

typedef struct {
    LV2_URID atom_Object;
    LV2_URID atom_Float;
    LV2_URID atom_Vector;
    LV2_URID atom_URID;
    LV2_URID atom_eventTransfer;
} OSCLV2URIs;

static inline void map_osclv2_uris(LV2_URID_Map* map, OSCLV2URIs* uris) {
    uris->atom_Object             = map->map(map->handle, LV2_ATOM__Object);
    uris->atom_Float              = map->map(map->handle, LV2_ATOM__Float);
    uris->atom_Vector             = map->map(map->handle, LV2_ATOM__Vector);
    uris->atom_URID               = map->map(map->handle, LV2_ATOM__URID);
    uris->atom_eventTransfer      = map->map(map->handle, LV2_ATOM__eventTransfer);
}

#endif //SRC_HEADERS_GXEFFECTS_H_
