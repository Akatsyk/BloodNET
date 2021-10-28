#include "../external_include.h"
#include "../picture_data.h"

c_window::c_window(std::string name, bounds pbounds, bool tabs)
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_hastabs = tabs;
	this->m_tabarea = bounds(pbounds.x + 201, pbounds.y + 44, 40, 30);
	this->m_subtabarea = bounds(pbounds.x + 1, pbounds.y + 78, /*201*/220, pbounds.h - 85);
	this->m_area = bounds(pbounds.x + 230, pbounds.y + 78, pbounds.w - 237, pbounds.h - 85);
}

void c_window::render()
{
	//do here cuz gay
	handle_movement();

	int center = m_bounds.x + m_bounds.w / 2;

	/*
	* AkatsukiSun: Fatality menu colours:
	*	1. Background tabs: 27,21,55
	*	2. Gradient line:
	*		1) 34,22,96
	*		2) 98,15,70
	*	3. Under tabs area: 12,9,26
	*	4. Subtub area: 31,25,66
	*	5. Non workable area: 27,19,54
	*	6. Workable area: 33,26,67
	*/

	render::get().filled_box(m_bounds.x, m_bounds.y, m_bounds.w, m_bounds.h, D3DCOLOR_RGBA(15, 10, 25, 250));

	render::get().bordered_box(m_bounds.x, m_bounds.y, m_bounds.w, m_bounds.h, D3DCOLOR_RGBA(60, 55, 95, 250), 1);

	render::get().rect_filled_linear_gradient(Vector2D(m_bounds.x + 1, m_bounds.y + 20), Vector2D(m_bounds.w - 2, 2), D3DCOLOR_RGBA(35, 20, 100, 250), D3DCOLOR_RGBA(100, 15, 70, 250), true);

	render::get().filled_box(m_area.x, m_area.y, m_area.w, m_area.h, D3DCOLOR_RGBA(35, 25, 70, 250));

	render::get().filled_box(m_subtabarea.x, m_bounds.y + 1, m_bounds.w - 2, 18, D3DCOLOR_RGBA(30, 20, 55, 250));

	render::get().filled_box(m_subtabarea.x, m_bounds.y + 22, m_bounds.w - 2, 50, D3DCOLOR_RGBA(30, 20, 55, 250));

	render::get().filled_box(m_subtabarea.x + 5, m_subtabarea.y, m_subtabarea.w - 5, m_subtabarea.h, D3DCOLOR_RGBA(30, 25, 65, 250));

	static float Blue = 0.4f;
	static float Red = 0.1f;
	static bool DragR = false;
	static bool DragB = false;
	Blue -= 0.01f;
	if (Blue < 0.f)
	{
		Blue += 1.f;
		DragB = DragR;
	}
	Red -= 0.01f;
	if (Red < 0.f)
	{
		Red += 1.f;
		DragR = !DragB;
	}

	{
		render::get().text(DragB ? Vector2D(m_bounds.x + 21, m_tabarea.y - 23) : Vector2D(m_bounds.x + 19, m_tabarea.y - 21),
			"BLOODNET", Color(10, 10, 255, Blue * 200.f), fonts::menu_new_font, c_font::left_aligned);

		render::get().text(DragR ? Vector2D(m_bounds.x + 19, m_tabarea.y - 21) : Vector2D(m_bounds.x + 23, m_tabarea.y - 23),
			"BLOODNET", Color(255, 10, 10, Red * 200.f), fonts::menu_new_font, c_font::left_aligned);
	}

	DragB = !DragB;

	render::get().text(Vector2D(m_bounds.x + 20, m_tabarea.y - 22),
		"BLOODNET", Color(255, 255, 255, 255), fonts::menu_new_font, c_font::left_aligned);

	for (auto tab : m_tabs)
	{
		if (tab->update())
			m_selected_tab = tab;

		tab->render((m_selected_tab == tab));
	}
}

void c_window::handle_movement()
{
	if (c_menu::is_holding(bounds(m_bounds.x, m_bounds.y, m_bounds.w, 24)) && !m_is_dragged)
	{
		m_drag_offset.x = render::get().get_dimensions().Width - c_menu::get().get_mouse_pos().x;
		m_drag_offset.y = render::get().get_dimensions().Height - c_menu::get().get_mouse_pos().y;
		m_is_dragged = true;
	}
	else if (!c_menu::mouse_is_holding())
		m_is_dragged = false;

	if (!m_is_dragged)
		return;

	POINT offset;
	offset.x = render::get().get_dimensions().Width - c_menu::get().get_mouse_pos().x;
	offset.y = render::get().get_dimensions().Height - c_menu::get().get_mouse_pos().y;

	POINT dif;
	dif.x = offset.x - m_drag_offset.x;
	dif.y = offset.y - m_drag_offset.y;

	auto resultx = m_bounds.x - dif.x;
	auto resulty = m_bounds.y - dif.y;

	if (resultx < 0 || resultx + m_bounds.w > render::get().get_dimensions().Width)
	{
		dif.x = 0;
		resultx = m_bounds.x;
	}

	if (resulty < 0 || resulty + m_bounds.h > render::get().get_dimensions().Height)
	{
		dif.y = 0;
		resulty = m_bounds.y;
	}

	update_controls(-dif.x, -dif.y);

	m_bounds.x = resultx;
	m_bounds.y = resulty;

	m_tabarea = bounds(m_bounds.x + 201, m_bounds.y + 44, 40, 30);
	m_subtabarea = bounds(m_bounds.x + 1, m_bounds.y + 78, /*201*/220, m_bounds.h - 85);
	m_area = bounds(m_bounds.x + 230, m_bounds.y + 78, m_bounds.w - 237, m_bounds.h - 85);

	m_drag_offset.x = render::get().get_dimensions().Width - c_menu::get().get_mouse_pos().x;
	m_drag_offset.y = render::get().get_dimensions().Height - c_menu::get().get_mouse_pos().y;
}

void c_window::update_controls(LONG xdrag, LONG ydrag)
{
	for (auto& tab : m_tabs)
		for (auto& subtab : tab->m_subtabs)
			for (auto& control : subtab->m_controls)
			{
				control->update_pos(xdrag, ydrag);
				control->update_pos_individual(xdrag, ydrag);
			}
}


c_groupwindow* c_window::add_groupwindow(std::string name, bounds bound)
{
	//return new c_groupwindow(name, bounds(m_area.x + bound.x, m_area.y + bound.y, bound.w, bound.h));
	return nullptr;
}

void c_window::pushback_groupwindow(c_groupwindow* window)
{
	m_groups.push_back(window);
}

c_tab* c_window::add_tab(std::string name)
{
	return new c_tab(name, m_tabamount++, this);
}

void c_window::pushback_tab(c_tab* tab)
{
	m_tabs.push_back(tab);
}

void c_window::set_active(bool active)
{
	m_active = active;
}
