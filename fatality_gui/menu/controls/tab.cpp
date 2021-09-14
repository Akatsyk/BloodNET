﻿#include "../external_include.h"

c_tab::c_tab(std::string name, int id, c_window* window)
{
	this->m_name = name;
	this->m_id = id;
	this->parent_window = window;
}

void c_tab::render(bool selected)
{
	bool hovered = c_menu::get().is_hovering(bounds(parent_window->m_tabarea.x + 1, parent_window->m_tabarea.y + 32 + 45 * m_id + add, parent_window->m_tabarea.w - 1, 45));

	_u(name, m_name);
	if (selected)
	{
		render::get().rect_filled_diamond_gradient(Vector2D(parent_window->m_tabarea.x, parent_window->m_tabarea.y + 32 + 45 * m_id + add), Vector2D(parent_window->m_tabarea.w, 45), D3DCOLOR_RGBA(33, 27, 70, 250), D3DCOLOR_RGBA(235, 5, 90, 250));
		render::get().text(Vector2D(parent_window->m_tabarea.x + (parent_window->m_tabarea.w / 2),
			parent_window->m_tabarea.y + 32 + 45 * m_id + add + 22 - (render::get().get_text_size(name, fonts::tabfontthicc).y / 2)),
			name, Color(215, 240, 250, 250), fonts::tabfontthicc, c_font::centered_x);
	}
	else
	{
		render::get().rect_filled_diamond_gradient(Vector2D(parent_window->m_tabarea.x, parent_window->m_tabarea.y + 32 + 45 * m_id + add), Vector2D(parent_window->m_tabarea.w, 45), D3DCOLOR_RGBA(33, 27, 70, 250), hovered ? D3DCOLOR_RGBA(154, 10, 79, 250) : D3DCOLOR_RGBA(33, 27, 70, 250));
		render::get().text(Vector2D(parent_window->m_tabarea.x + (parent_window->m_tabarea.w / 2),
			parent_window->m_tabarea.y + 32 + 45 * m_id + add + 22 - (render::get().get_text_size(name, fonts::tabfont).y / 2)),
			name, Color(210, 220, 205, 250), fonts::tabfont, c_font::centered_x);
	}

	if (selected)
	{
		for (auto tab : m_subtabs)
		{
			if (tab->update())
				m_selected_subtab = tab;

			tab->render((m_selected_subtab == tab));
		}
	}
	//bool hovered = c_menu::get().is_hovering(bounds(parent_window->m_tabarea.x + 1, parent_window->m_tabarea.y + 32 + 45 * m_id + add, parent_window->m_tabarea.w - 1, 45));

	//_u(name, m_name);
	//if (selected)
	//{
	//	render::get().rect_filled_diamond_gradient(Vector2D(parent_window->m_tabarea.x, parent_window->m_tabarea.y + 32 + 45 * m_id + add), Vector2D(parent_window->m_tabarea.w, 45), D3DCOLOR_RGBA(33, 31, 36, 250), D3DCOLOR_RGBA(100, 100, 100, 250));
	//	render::get().text(Vector2D(parent_window->m_tabarea.x + (parent_window->m_tabarea.w / 2),
	//		parent_window->m_tabarea.y + 32 + 45 * m_id + add + 22 - (render::get().get_text_size(name, fonts::tabfontthicc).y / 2)),
	//		name, Color(210, 220, 205, 255), fonts::tabfontthicc, c_font::centered_x);
	//}
	//else
	//{
	//	render::get().rect_filled_diamond_gradient(Vector2D(parent_window->m_tabarea.x, parent_window->m_tabarea.y + 32 + 45 * m_id + add), Vector2D(parent_window->m_tabarea.w, 45), D3DCOLOR_RGBA(33, 31, 36, 250), hovered ? D3DCOLOR_RGBA(154, 10, 79, 250) : D3DCOLOR_RGBA(33, 31, 36, 250));
	//	render::get().text(Vector2D(parent_window->m_tabarea.x + (parent_window->m_tabarea.w / 2),
	//		parent_window->m_tabarea.y + 32 + 45 * m_id + add + 22 - (render::get().get_text_size(name, fonts::tabfont).y / 2)),
	//		name, Color(210, 220, 205, 255), fonts::tabfont, c_font::centered_x);
	//}

	//if (selected)
	//{
	//	for (auto tab : m_subtabs)
	//	{
	//		if (tab->update())
	//			m_selected_subtab = tab;

	//		tab->render((m_selected_subtab == tab));
	//	}
	//}
}

bool c_tab::update()
{
	return c_menu::get().is_pressed(bounds(parent_window->m_tabarea.x + 1,
		parent_window->m_tabarea.y + 32 + 45 * m_id + add, parent_window->m_tabarea.w - 1, 45));
}

bool c_tab::is_selected()
{
	return *m_selected;
}

c_subtab* c_tab::add_subtab(std::string name)
{
	return new c_subtab(name, m_tabamount++, parent_window, this);
}

void c_tab::pushback_subtab(c_subtab* subtab)
{
	m_subtabs.push_back(subtab);
}