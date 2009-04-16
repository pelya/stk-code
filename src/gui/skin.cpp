#include "gui/skin.hpp"
#include "gui/engine.hpp"
#include "gui/screen.hpp"
#include "gui/widget.hpp"
#include "io/file_manager.hpp"
#include <cassert>
#include <iostream>

using namespace GUIEngine;

Skin::Skin(IGUISkin* fallback_skin)
{
    m_fallback_skin = fallback_skin;
    m_fallback_skin->grab();
    assert(fallback_skin != NULL);
    
    
    m_tex_button = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glassbutton.png").c_str() );
    m_tex_fbutton = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glassbutton_focused.png").c_str() );
    
    m_tex_spinner = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glassspinner.png").c_str() );
    m_tex_fspinner = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glassspinner_focus.png").c_str() );
    m_tex_dspinner = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glassspinner_down.png").c_str() );
    
    m_tex_tab = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glasstab.png").c_str() );
    m_tex_ftab = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glasstab_focus.png").c_str() );
    m_tex_dtab = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glasstab_down.png").c_str() );
    
    m_tex_iconhighlight = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glass_iconhighlight.png").c_str() );
    m_tex_ficonhighlight = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glass_iconhighlight_focus.png").c_str() );
    m_tex_squarefocus = GUIEngine::getDriver()->getTexture( (file_manager->getGUIDir() + "/glass_square_focused.png").c_str() );
    
}

Skin::~Skin()
{
    m_fallback_skin->drop();
}

/*
 driver->draw2DImage(texture,
 core::rect<s32>(img_x1, img_y1, img_x2, img_y2),
 core::rect<s32>(0,0,texture_size.Width, texture_size.Height) );
 */ 
void Skin::draw2DRectangle (IGUIElement *element, const video::SColor &color, const core::rect< s32 > &pos, const core::rect< s32 > *clip)
{
    
    // scrollbar backgound
    
    //printf("draw rectangle\n");
    GUIEngine::getDriver()->draw2DRectangle( SColor(255, 0, 150, 150), pos );
}

void Skin::drawBoxFromStretchableTexture(const core::rect< s32 > &dest, ITexture* source,
                                         const int left_border, const int right_border,
                                         const int top_border, const int bottom_border,
                                         const float border_out_portion, int areas)
{
    // FIXME? - lots of things here will be re-calculated every frame, which is useless since
    // widgets won't move, so they'd only need to be calculated once.
    const int texture_w = source->getSize().Width;
    const int texture_h = source->getSize().Height;
    
    
    /*
     The source texture is split this way to allow for a stretchable center and borders that don't stretch :
     
     +----+--------------------+----+
     |    |                    |    |
     +----a--------------------b----+  <-- top_border
     |    |                    |    |
     |    |                    |    |     
     |    |                    |    |     
     +----c--------------------d----+ <-- height - bottom-border
     |    |                    |    | 
     +----+--------------------+----+
     */
    
    const int ax = left_border;
    const int ay = top_border;
    const int bx = texture_w - right_border;
    const int by = top_border;
    const int cx = left_border;
    const int cy = texture_h - bottom_border;
    const int dx = texture_w - right_border;
    const int dy = texture_h - bottom_border;
    
    core::rect<s32> source_area_left = core::rect<s32>(0, ay, cx, cy);
    core::rect<s32> source_area_center = core::rect<s32>(ax, ay, dx, dy);
    core::rect<s32> source_area_right = core::rect<s32>(bx, top_border, texture_w, dy);
    
    core::rect<s32> source_area_top = core::rect<s32>(ax, 0, bx, by);
    core::rect<s32> source_area_bottom = core::rect<s32>(cx, cy, dx, texture_h);
    
    core::rect<s32> source_area_top_left = core::rect<s32>(0, 0, ax, ay);
    core::rect<s32> source_area_top_right = core::rect<s32>(bx, 0, texture_w, top_border);
    core::rect<s32> source_area_bottom_left = core::rect<s32>(0, cy, cx, texture_h);
    core::rect<s32> source_area_bottom_right = core::rect<s32>(dx, dy, texture_w, texture_h);
    
    /*
     The dest area is split this way. Borders can go a bit beyond the given area so
     components inside don't go over the borders
     (how much it exceeds horizontally is specified in 'border_out_portion'. vertically is always the totality)
     
     a----b--------------------c----+
     |    |                    |    |
     d----e--------------------f----g  <-- top_border
     |    |                    |    |
     |    |                    |    |     
     |    |                    |    |     
     h----i--------------------j----k  <-- height - bottom-border
     |    |                    |    | 
     +----l--------------------m----n
     */
    {
        const int dest_x = dest.UpperLeftCorner.X;
        const int dest_y = dest.UpperLeftCorner.Y;
        const int dest_x2 = dest.LowerRightCorner.X;
        const int dest_y2 = dest.LowerRightCorner.Y;
        
        const float xscale = (float)(dest_x2-dest_x)/texture_w;
        const float yscale = (float)(dest_y2-dest_y)/texture_h;

        const int dest_left_border   = (int)(left_border  *std::min<float>(yscale, 1.0));
        const int dest_right_border  = (int)(right_border *std::min<float>(yscale, 1.0));
        const int dest_top_border    = (int)(top_border   *std::min<float>(xscale, 1.0));
        const int dest_bottom_border = (int)(bottom_border*std::min<float>(xscale, 1.0));
        
        const float border_in_portion = 1 - border_out_portion;
        
        const int ax = (int)(dest_x - dest_left_border*border_out_portion);
        const int ay = dest_y - dest_top_border;
        
        const int bx = (int)(dest_x + dest_left_border*border_in_portion);
        const int by = ay;
        
        const int cx = (int)(dest_x2 - dest_right_border*border_in_portion);
        const int cy = ay;
        
        const int dx = ax;
        const int dy = dest_y;
        
        const int ex = bx;
        const int ey = dy;
        
        const int fx = cx;
        const int fy = dy;
        
        const int gx = (int)(dest_x2 + dest_right_border*border_out_portion);
        const int gy = dy;
        
        const int hx = ax;
        const int hy = dest_y2;
        
        const int ix = bx;
        const int iy = hy;
        
        const int jx = cx;
        const int jy = hy;
        
        const int kx = gx;
        const int ky = hy;
        
        const int lx = bx;
        const int ly = dest_y2 + dest_bottom_border; 
        
        const int mx = cx;
        const int my = ly;
        
        const int nx = gx;
        const int ny = ly;
        
        core::rect<s32> dest_area_left = core::rect<s32>(dx, dy, ix, iy);
        core::rect<s32> dest_area_center = core::rect<s32>(ex, ey, jx, jy);
        core::rect<s32> dest_area_right = core::rect<s32>(fx, fy, kx, ky);
        
        core::rect<s32> dest_area_top = core::rect<s32>(bx, by, fx, fy);
        core::rect<s32> dest_area_bottom = core::rect<s32>(ix, iy, mx, my);
        
        core::rect<s32> dest_area_top_left = core::rect<s32>(ax, ay, ex, ey);
        core::rect<s32> dest_area_top_right = core::rect<s32>(cx, cy, gx, gy);
        core::rect<s32> dest_area_bottom_left = core::rect<s32>(hx, hy, lx, ly);
        core::rect<s32> dest_area_bottom_right = core::rect<s32>(jx, jy, nx, ny);
        
        if((areas & LEFT) != 0)
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_left, source_area_left,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        
        if((areas & BODY) != 0)
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_center, source_area_center,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        
        if((areas & RIGHT) != 0)
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_right, source_area_right,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        
        if((areas & TOP) != 0)
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_top, source_area_top,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        if((areas & BOTTOM) != 0)
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_bottom, source_area_bottom,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        
        if( ((areas & LEFT) != 0) && ((areas & TOP) != 0) )
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_top_left, source_area_top_left,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        if( ((areas & RIGHT) != 0) && ((areas & TOP) != 0) )
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_top_right, source_area_top_right,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        if( ((areas & LEFT) != 0) && ((areas & BOTTOM) != 0) )
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_bottom_left, source_area_bottom_left,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        if( ((areas & RIGHT) != 0) && ((areas & BOTTOM) != 0) )
        {
            GUIEngine::getDriver()->draw2DImage(source, dest_area_bottom_right, source_area_bottom_right,
                                                0 /* no clipping */, 0, true /* alpha */);
        }
        
    }
}

void Skin::drawButton(const core::rect< s32 > &rect, const bool pressed, const bool focused)
{
    // FIXME - move these numbers to a config file
    const int left_border = 80;
    const int right_border = 80;
    const int border_above = 0;
    const int border_below = 36;
    
    drawBoxFromStretchableTexture(rect, (focused ? m_tex_fbutton : m_tex_button),
                                  left_border, right_border,
                                  border_above, border_below);
    
    /* 
     if(focused)
     GUIEngine::getDriver()->draw2DRectangle( SColor(255, 100, 0, 0), rect );
     else
     GUIEngine::getDriver()->draw2DRectangle( SColor(255, 0, 100, 0), rect );
     */
}
void Skin::drawRibbon(const core::rect< s32 > &rect, const Widget* widget, const bool pressed, bool focused)
{
    return;
    
    // only combo ribbons need a border
    if ( ((RibbonWidget*)widget)->getRibbonType() != RIBBON_COMBO ) return;
    
    bool draw_border = focused;
    
    // check if one of its children has focus (will happen when directly clicking on them)
    const int amount = widget->m_children.size();
    for(int n=0; n<amount; n++)
    {
        if(GUIEngine::getGUIEnv()->hasFocus(widget->m_children[n].m_element))
        {
            draw_border = true;
            focused = true;
            break;
        }
    }
    
    if(!draw_border) return;
    
    if(focused)
        GUIEngine::getDriver()->draw2DRectangle( SColor(255, 150, 0, 0), rect );
    else
        GUIEngine::getDriver()->draw2DRectangle( SColor(255, 0, 150, 0), rect );
}

void Skin::drawRibbonChild(const core::rect< s32 > &rect, const Widget* widget, const bool pressed, bool focused)
{
    bool mark_selected = widget->isSelected();
    
    const bool parent_focused = GUIEngine::getGUIEnv()->getFocus() == widget->m_parent->m_element;
    
    /* tab-bar ribbons */
    if(widget->m_type == WTYPE_BUTTON)
    {
        // ribbons containing buttons are actually tabs
        
        // FIXME - specify in file, don't hardcode
        const int left_border = 75;
        const int right_border = 75;
        const int border_above = 0;
        const int border_below = 0;
        
        if (mark_selected)
        {
            core::rect< s32 > rect2 = rect;
            rect2.LowerRightCorner.Y += 10;
            
            drawBoxFromStretchableTexture(rect2, (focused || parent_focused ? m_tex_ftab : m_tex_dtab),
                                          left_border, right_border,
                                          border_above, border_below, 0.2f);
        }
        else
        {
            drawBoxFromStretchableTexture(rect, m_tex_tab,
                                          left_border, right_border,
                                          border_above, border_below, 0);
        }
        
    }
    /* icon ribbons */
    else
    {
        bool use_glow = true;

        if (widget->m_parent != NULL && widget->m_parent->m_properties[PROP_SQUARE] == "true") use_glow = false;
        if (widget->m_parent != NULL && widget->m_parent->m_parent != NULL &&
            widget->m_parent->m_parent->m_properties[PROP_SQUARE] == "true") use_glow = false;
        
        
        if(widget->m_parent != NULL && widget->m_parent->m_type == WTYPE_RIBBON && 
           ((RibbonWidget*)widget->m_parent)->getRibbonType() == RIBBON_TOOLBAR &&
           !GUIEngine::getGUIEnv()->hasFocus(widget->m_parent->m_element)) mark_selected = false;
        
        if(mark_selected)
        {
            if (use_glow)
            {
                int grow = 45;
                static float glow_effect = 0;
                
                if(focused || parent_focused)
                {
                    const float dt = GUIEngine::getLatestDt();
                    glow_effect += dt*3;
                    if (glow_effect > 6.2832f /* 2*PI */) glow_effect -= 6.2832f;
                    grow = (int)(45 + 10*sin(glow_effect));
                }
                
                
                const int glow_center_x = rect.UpperLeftCorner.X + rect.getWidth()/2;
                const int glow_center_y = rect.UpperLeftCorner.Y + rect.getHeight() - 5;
                
                const core::rect< s32 > rect2 =  core::rect< s32 >(glow_center_x - 45 - grow,
                                                                   glow_center_y - 25 - grow/2,
                                                                   glow_center_x + 45 + grow,
                                                                   glow_center_y + 25 + grow/2);
                
                // GUIEngine::getDriver()->draw2DRectangle( SColor(2555, 0, 175, 255), rect2 );
                
                const int texture_w = m_tex_ficonhighlight->getSize().Width;
                const int texture_h = m_tex_ficonhighlight->getSize().Height;
                
                core::rect<s32> source_area = core::rect<s32>(0, 0, texture_w, texture_h);
                
                if(!focused && !parent_focused)
                {
                    GUIEngine::getDriver()->draw2DImage(m_tex_iconhighlight, rect2, source_area,
                                                        0 /* no clipping */, 0, true /* alpha */);
                }
                else
                {
                    GUIEngine::getDriver()->draw2DImage(m_tex_ficonhighlight, rect2, source_area,
                                                        0 /* no clipping */, 0, true /* alpha */);
                }
            }
            /* if we're not using glow */
            else
            {
                const int texture_w = m_tex_squarefocus->getSize().Width;
                const int texture_h = m_tex_squarefocus->getSize().Height;
                
                core::rect<s32> source_area = core::rect<s32>(0, 0, texture_w, texture_h);
                
                drawBoxFromStretchableTexture(rect, m_tex_squarefocus,
                                              6 /* left border */, 6 /* rightborder */,
                                              6 /* top border */, 6 /* bottom border */, 1);
            }
            
        }
    }
    
}

void Skin::drawSpinnerBody(const core::rect< s32 > &rect, const Widget* widget, const bool pressed, bool focused)
{
    // FIXME - move these numbers to a config file
    const int left_border = 110;
    const int right_border = 110;
    const int border_above = 0;
    const int border_below = 36;
    
    if(!focused)
    {
        IGUIElement* focused_widget = GUIEngine::getGUIEnv()->getFocus();
        
        //{
        //Widget* widget = GUIEngine::getCurrentScreen()->getWidget(focused_widget->getID());
        //if(focused_widget == NULL) std::cout << "no focus\n";
        //else if(widget != NULL)
        //{
        //    std::cout << "focused : " << widget->m_properties[PROP_ID].c_str() << " (id " << focused_widget->getID() << ")" << std::endl;
        //}
        //}

        //Widget* stupid_stl = (Widget*)widget;
        //std::cout << "widget name : " << (stupid_stl->m_properties[PROP_ID].c_str()) << std::endl;
        
        if(focused_widget != NULL && widget->m_children.size()>2)
        {
            //std::cout << "checking against " << widget->m_children[0].id << " and " << widget->m_children[2].id << std::endl;
            if(widget->m_children[0].id == focused_widget->getID() ||
               widget->m_children[2].id == focused_widget->getID())
            {
                //std::cout << "focused = true;\n";
                focused = true;
            }
        }
    }
    
    drawBoxFromStretchableTexture(rect, (focused || pressed ? m_tex_fspinner : m_tex_spinner),
                                  left_border, right_border,
                                  border_above, border_below, 0);
}

void Skin::drawSpinnerChild(const core::rect< s32 > &rect, Widget* widget, const bool pressed, bool focused)
{
    // FIXME - move these numbers to a config file
    const int left_border = 110;
    const int right_border = 110;
    const int border_above = 0;
    const int border_below = 36;
    
    if(pressed)
    {
        Widget* spinner = widget->m_parent;
        int areas = 0;
        
        //std::cout << "drawing spinner child " << widget->m_properties[PROP_ID].c_str() << std::endl;
        
        if (widget->m_properties[PROP_ID] == "left") areas = LEFT;
        else if (widget->m_properties[PROP_ID] == "right") areas = RIGHT;
        else return;
        
        core::rect< s32 > rect2 = core::rect< s32 >( spinner->x, spinner->y,
                                                    spinner->x + spinner->w,
                                                    spinner->y + spinner->h  );
        
        //std::cout << "proceeding to render " << areas << std::endl;
        
        drawBoxFromStretchableTexture(rect2, m_tex_fspinner,
                                      left_border, right_border,
                                      border_above, border_below, 0, LEFT | RIGHT);
        
        drawBoxFromStretchableTexture(rect2, m_tex_dspinner,
                                      left_border, right_border,
                                      border_above, border_below, 0, areas);
        
    }
    
}

void Skin::process3DPane(IGUIElement *element, const core::rect< s32 > &rect, const bool pressed)
{
    const bool focused = GUIEngine::getGUIEnv()->hasFocus(element);
    
    const int id = element->getID();
    
    Widget* widget = GUIEngine::getCurrentScreen()->getWidget(id);
    
    if(widget == NULL) return;
    
    const WidgetType type = widget->m_type;
    
    
    // buttons are used for other uses than plain clickable buttons because irrLicht
    // does not have widgets for everything we need. so at render time, we just check
    // which type this button represents and render accordingly
    
    if(widget->m_parent != NULL && widget->m_parent->m_type == WTYPE_RIBBON)
    {
        drawRibbonChild(rect, widget, pressed /* pressed */, focused /* focused */);
    }
    else if(widget->m_parent != NULL && widget->m_parent->m_type == WTYPE_SPINNER)
    {
        drawSpinnerChild(rect, widget, pressed /* pressed */, focused /* focused */);
    }
    else if(type == WTYPE_ICON_BUTTON)
    {
        if(!focused) return; /* don't draw any border in this case */
        else drawButton(rect, pressed /* pressed */, true /* focused */);
    }
    else if(type == WTYPE_BUTTON)
    {
        drawButton(rect, pressed, focused);
    }
    else if(type == WTYPE_RIBBON)
    {
        drawRibbon(rect, widget, pressed, focused);
    }
    else if(type == WTYPE_SPINNER)
    {
        drawSpinnerBody(rect, widget, pressed, focused);
    }
    
}

void Skin::draw3DButtonPanePressed (IGUIElement *element, const core::rect< s32 > &rect, const core::rect< s32 > *clip)
{
    process3DPane(element, rect, true /* pressed */ );
}

void Skin::draw3DButtonPaneStandard (IGUIElement *element, const core::rect< s32 > &rect, const core::rect< s32 > *clip)
{
    process3DPane(element, rect, false /* pressed */ );
}

void Skin::draw3DMenuPane (IGUIElement *element, const core::rect< s32 > &rect, const core::rect< s32 > *clip)
{
    //printf("draw menu pane\n");
}

void Skin::draw3DSunkenPane (IGUIElement *element, video::SColor bgcolor, bool flat, bool fillBackGround, const core::rect< s32 > &rect, const core::rect< s32 > *clip)
{
    // e.g. the checkbox square
    //printf("draw sunken pane\n");
    const bool focused = GUIEngine::getGUIEnv()->getFocus() == element;
    
    if(focused)
        GUIEngine::getDriver()->draw2DRectangle( SColor(255, 150, 0, 0), rect );
    else
        GUIEngine::getDriver()->draw2DRectangle( SColor(255, 0, 150, 0), rect );
}

void Skin::draw3DTabBody (IGUIElement *element, bool border, bool background, const core::rect< s32 > &rect, const core::rect< s32 > *clip, s32 tabHeight, gui::EGUI_ALIGNMENT alignment)
{
    //printf("draw tab body\n");
}

void Skin::draw3DTabButton (IGUIElement *element, bool active, const core::rect< s32 > &rect, const core::rect< s32 > *clip, gui::EGUI_ALIGNMENT alignment)
{
    //printf("draw tab button\n");
}

void Skin::draw3DToolBar (IGUIElement *element, const core::rect< s32 > &rect, const core::rect< s32 > *clip)
{
    //printf("draw toolbar\n");
}

core::rect< s32 > Skin::draw3DWindowBackground (IGUIElement *element, bool drawTitleBar, video::SColor titleBarColor, const core::rect< s32 > &rect, const core::rect< s32 > *clip)
{
    //printf("draw 3d window bg\n");
    return rect;
}

void Skin::drawIcon (IGUIElement *element, EGUI_DEFAULT_ICON icon, const core::position2di position, u32 starttime, u32 currenttime, bool loop, const core::rect< s32 > *clip)
{
    //printf("draw icon\n");
    m_fallback_skin->drawIcon(element, icon, position, starttime, currenttime, loop, clip);
}

video::SColor Skin::getColor (EGUI_DEFAULT_COLOR color) const 
{
    //printf("getting color\n");
    /*
     EGDC_3D_DARK_SHADOW 	Dark shadow for three-dimensional display elements.
     EGDC_3D_SHADOW 	Shadow color for three-dimensional display elements (for edges facing away from the light source).
     EGDC_3D_FACE 	Face color for three-dimensional display elements and for dialog box backgrounds.
     EGDC_3D_HIGH_LIGHT 	Highlight color for three-dimensional display elements (for edges facing the light source.).
     EGDC_3D_LIGHT 	Light color for three-dimensional display elements (for edges facing the light source.).
     EGDC_ACTIVE_BORDER 	Active window border.
     EGDC_ACTIVE_CAPTION 	Active window title bar text.
     EGDC_APP_WORKSPACE 	Background color of multiple document interface (MDI) applications.
     EGDC_BUTTON_TEXT 	Text on a button.
     EGDC_GRAY_TEXT 	Grayed (disabled) text.
     EGDC_HIGH_LIGHT 	Item(s) selected in a control.
     EGDC_HIGH_LIGHT_TEXT 	Text of item(s) selected in a control.
     EGDC_INACTIVE_BORDER 	Inactive window border.
     EGDC_INACTIVE_CAPTION 	Inactive window caption.
     EGDC_TOOLTIP 	Tool tip text color.
     EGDC_TOOLTIP_BACKGROUND 	Tool tip background color.
     EGDC_SCROLLBAR 	Scrollbar gray area.
     EGDC_WINDOW 	Window background.
     EGDC_WINDOW_SYMBOL 	Window symbols like on close buttons, scroll bars and check boxes.
     EGDC_ICON 	Icons in a list or tree.
     EGDC_ICON_HIGH_LIGHT 	Selected icons in a list or tree. 
     */
    
    switch(color)
    {
        case EGDC_BUTTON_TEXT:
            return SColor(255, 0, 0, 0);
            break;
            
        case EGDC_GRAY_TEXT:
            return SColor(255, 80, 80, 80);
            break;
            
        case EGDC_HIGH_LIGHT:
        case EGDC_ICON_HIGH_LIGHT:
        case EGDC_HIGH_LIGHT_TEXT:
            return SColor(255, 0, 150, 0);
            
        default:
            return SColor(255, 255, 255, 255);
    }
    
}

const wchar_t* 	Skin::getDefaultText (EGUI_DEFAULT_TEXT text) const 
{
    //printf("getting default text\n");
    return L"SuperTuxKart";
}

IGUIFont* Skin::getFont (EGUI_DEFAULT_FONT which) const 
{
    //printf("getting font\n");
    return GUIEngine::getFont();
}

u32 Skin::getIcon (EGUI_DEFAULT_ICON icon) const 
{
    //printf("getting icon\n");
    return m_fallback_skin->getIcon(icon);
}

s32 Skin::getSize (EGUI_DEFAULT_SIZE size) const 
{
    //printf("getting size\n");
    return m_fallback_skin->getSize(size);
}

IGUISpriteBank* Skin::getSpriteBank () const 
{
    //printf("getting bank\n");
    return m_fallback_skin->getSpriteBank();
}

//EGUI_SKIN_TYPE 	getType () const

void Skin::setColor (EGUI_DEFAULT_COLOR which, video::SColor newColor)
{
    m_fallback_skin->setColor(which, newColor);
    //printf("setting color\n");
}

void Skin::setDefaultText (EGUI_DEFAULT_TEXT which, const wchar_t *newText)
{
    m_fallback_skin->setDefaultText(which, newText);
    //printf("setting default text\n");
}

void Skin::setFont (IGUIFont *font, EGUI_DEFAULT_FONT which)
{
    m_fallback_skin->setFont(font, which);
    //printf("setting font\n");
}

void Skin::setIcon (EGUI_DEFAULT_ICON icon, u32 index)
{
    m_fallback_skin->setIcon(icon, index);
    //printf("setting icon\n");
}

void Skin::setSize (EGUI_DEFAULT_SIZE which, s32 size)
{
    m_fallback_skin->setSize(which, size);
    //printf("setting size\n");
}

void Skin::setSpriteBank (IGUISpriteBank *bank)
{
    //printf("setting sprite bank\n");
    m_fallback_skin->setSpriteBank(bank);
    //this->m_bank = bank;
}
