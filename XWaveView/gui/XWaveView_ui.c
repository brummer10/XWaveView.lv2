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


#define CONTROLS 1

#define GUI_ELEMENTS 0

#define TAB_ELEMENTS 0

#define OSCSIZE 256

#define RMSSIZE 16

#include "XWaveView.h"


#include "lv2_plugin.h"

typedef struct {
    LV2_Atom_Forge forge;

    OSCLV2URIs   uris;
    Widget_t* osc_scale;
    float *osc;

} X11_UI_Private_t;

void set_costum_theme(Xputty *main) {
    main->color_scheme->normal = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.150, 0.150, 0.550, 1.000},
        .bg =       { 0.100, 0.100, 0.100, 1.000},
        .base =     { 0.000, 0.000, 0.000, 1.000},
        .text =     { 0.900, 0.900, 0.900, 1.000},
        .shadow =   { 0.000, 0.000, 0.000, 0.200},
        .frame =    { 0.000, 0.000, 0.000, 1.000},
        .light =    { 0.300, 0.300, 0.500, 1.000}
    };

    main->color_scheme->prelight = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 1.000, 0.000, 1.000, 1.000},
        .bg =       { 0.250, 0.250, 0.250, 1.000},
        .base =     { 0.300, 0.300, 0.300, 1.000},
        .text =     { 1.000, 1.000, 1.000, 1.000},
        .shadow =   { 0.100, 0.100, 0.100, 0.400},
        .frame =    { 0.300, 0.300, 0.300, 1.000},
        .light =    { 0.300, 0.300, 0.300, 1.000}
    };

    main->color_scheme->selected = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.900, 0.900, 0.900, 1.000},
        .bg =       { 0.200, 0.200, 0.200, 1.000},
        .base =     { 0.500, 0.180, 0.180, 1.000},
        .text =     { 1.000, 1.000, 1.000, 1.000},
        .shadow =   { 0.800, 0.180, 0.180, 0.200},
        .frame =    { 0.500, 0.180, 0.180, 1.000},
        .light =    { 0.500, 0.180, 0.180, 1.000}
    };

    main->color_scheme->active = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.000, 1.000, 1.000, 1.000},
        .bg =       { 0.000, 0.000, 0.000, 1.000},
        .base =     { 0.180, 0.380, 0.380, 1.000},
        .text =     { 0.750, 0.750, 0.750, 1.000},
        .shadow =   { 0.180, 0.380, 0.380, 0.500},
        .frame =    { 0.180, 0.380, 0.380, 1.000},
        .light =    { 0.180, 0.380, 0.380, 1.000}
    };

    main->color_scheme->insensitive = (Colors) {
         /* cairo    / r  / g  / b  / a  /  */
        .fg =       { 0.850, 0.850, 0.850, 0.500},
        .bg =       { 0.100, 0.100, 0.100, 0.500},
        .base =     { 0.000, 0.000, 0.000, 0.500},
        .text =     { 0.900, 0.900, 0.900, 0.500},
        .shadow =   { 0.000, 0.000, 0.000, 0.100},
        .frame =    { 0.000, 0.000, 0.000, 0.500},
        .light =    { 0.100, 0.100, 0.100, 0.500}
    };
}

#include "lv2_plugin.cc"


void rounded_waveview(cairo_t *cr,float x, float y, float w, float h, float lsize) {
    cairo_new_path (cr);
    float r = 20.0;
    cairo_move_to(cr, x+lsize+r,y);
    cairo_line_to(cr, x+w-r,y);
    cairo_curve_to(cr, x+w,y,x+w,y,x+w,y+r);
    cairo_line_to(cr, x+w,y+h-r);
    cairo_curve_to(cr, x+w,y+h,x+w,y+h,x+w-r,y+h);
    cairo_line_to(cr, x+r,y+h);
    cairo_curve_to(cr, x,y+h,x,y+h,x,y+h-r);
    cairo_line_to(cr, x,y+r);
    cairo_curve_to(cr, x,y,x,y,x+r,y);
}

void draw_waveview(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    WaveView_t *wave_view = (WaveView_t*)w->private_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width_t = attrs.width;
    int height_t = attrs.height;
    int half_height_t = height_t/2;

    cairo_text_extents_t extents;
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_set_line_width(w->crb,2);
    use_bg_color_scheme(w, NORMAL_);
    rounded_waveview(w->crb, 2, 5, width_t-4, height_t-7, extents.width+15);
    cairo_fill_preserve(w->crb);
    use_frame_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);
    cairo_move_to(w->crb,2,half_height_t);
    cairo_line_to(w->crb, width_t-4, half_height_t);
    cairo_stroke(w->crb);

    use_text_color_scheme(w, get_color_state(w));
    cairo_move_to (w->crb, 30, extents.height);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

    if (wave_view->size<1) return;
    float step = (float)(width_t-10)/(float)wave_view->size+1;
    float lstep = (float)(half_height_t-10.0);
    cairo_set_line_width(w->cr,2);
    cairo_move_to(w->crb, 2, half_height_t);
    int i = 0;
    for (;i<wave_view->size;i++) {
        cairo_line_to(w->crb, (float)(i+0.5)*step,(float)(half_height_t)+ -wave_view->wave[i]*lstep);
    }

    for (;i>-1;i--) {
        cairo_line_to(w->crb, (float)(i+0.5)*step,(float)(half_height_t)+ wave_view->wave[i]*lstep);
    }

    cairo_close_path(w->crb);
    use_light_color_scheme(w, NORMAL_);
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);
    cairo_move_to(w->crb, 2, half_height_t);
    cairo_line_to(w->crb, width_t, half_height_t);
    cairo_stroke(w->crb);
}

void plugin_value_changed(X11_UI *ui, Widget_t *w, PortIndex index) {
    // do special stuff when needed
}

void plugin_set_window_size(int *w,int *h,const char * plugin_uri) {
    (*w) = 320; //set initial width of main window
    (*h) = 260; //set initial height of main window
}

const char* plugin_set_name() {
    return "XWaveView"; //set plugin name to display on UI
}

void plugin_create_controller_widgets(X11_UI *ui, const char * plugin_uri) {
    X11_UI_Private_t *ps = (X11_UI_Private_t*)malloc(sizeof(X11_UI_Private_t));
    ps->osc = (float*) malloc(OSCSIZE* sizeof(float));
    map_osclv2_uris(ui->map, &ps->uris);
    lv2_atom_forge_init(&ps->forge, ui->map);
    ui->private_ptr = (void*)ps;
    set_costum_theme(&ui->main);
    ui->widget[0] = add_lv2_waveview (ui->widget[0], ui->win, NOTIFY, "RMS", ui, 10,  10, 300, 180);
    ui->widget[0]->scale.gravity = NORTHWEST;
    ui->widget[0]->func.expose_callback = draw_waveview;
    ps->osc_scale = add_knob(ui->win, "Scale", 260,190,50,65);
    ps->osc_scale->scale.gravity = SOUTHWEST;
    set_adjustment(ps->osc_scale->adj, 2.0, 2.0, 1.0, 4.0, 0.1, CL_CONTINUOS);
}

void plugin_cleanup(X11_UI *ui) {
    X11_UI_Private_t *ps = (X11_UI_Private_t*)ui->private_ptr;
    free(ps->osc);
    // clean up used sources when needed
}

void plugin_port_event(LV2UI_Handle handle, uint32_t port_index,
                        uint32_t buffer_size, uint32_t format,
                        const void * buffer) {
    X11_UI* ui = (X11_UI*)handle;
    X11_UI_Private_t *ps = (X11_UI_Private_t*)ui->private_ptr;
    OSCLV2URIs* uris = &ps->uris;
    if (format == ps->uris.atom_eventTransfer) {
        LV2_Atom* atom = (LV2_Atom*)buffer;
        if (atom->type == ps->uris.atom_Object) {
            LV2_Atom_Object* obj      = (LV2_Atom_Object*)atom;
            if (obj->body.otype == uris->atom_Float) {
                const LV2_Atom* vector_data = NULL;
                const int n_props  = lv2_atom_object_get(obj,uris->atom_Vector, &vector_data, NULL);
                if (!n_props) return;
                const LV2_Atom_Vector* vec = (LV2_Atom_Vector*)LV2_ATOM_BODY(vector_data);
                if (vec->atom.type == uris->atom_Float) {
                    int n_elem = (vector_data->size - sizeof(LV2_Atom_Vector_Body)) / vec->atom.size;
                    float* audio;
                    audio = (float*) LV2_ATOM_BODY(&vec->atom);
                    static int k = 0;
                    float sum = 0;
                    float v = adj_get_value(ps->osc_scale->adj);
                    int i = 0;
                    int j = 0;
                    for(; i < n_elem ; i++) {
                        sum += audio[i] * audio[i];
                        if(j == RMSSIZE) {
                            ps->osc[k] = sqrtf(sum / RMSSIZE)*v;
                            sum = 0;
                            k++;
                            j = 0;
                        }
                        if (k>OSCSIZE) {
                            update_waveview(ui->widget[0],ps->osc,OSCSIZE);
                            k = 0;
                        }
                        j++;
                    }
                    if (k>=OSCSIZE) {
                        update_waveview(ui->widget[0],ps->osc,OSCSIZE);
                        k = 0;
                    }
                }
            }
        }
    }
}

