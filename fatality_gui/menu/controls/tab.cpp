#include "../external_include.h"

c_tab::c_tab(std::string name, int id, c_window* window)
{
	this->m_name = name;
	this->m_id = id;
	this->parent_window = window;
}

void c_tab::render(bool selected)
{
	_u(name, m_name);

	bool hovered = c_menu::get().is_hovering(bounds(parent_window->m_tabarea.x + 110 * m_id + add,// - (render::get().get_text_size(name, fonts::tab_new_font).x / 2),
		parent_window->m_tabarea.y - 10,
		parent_window->m_tabarea.w,
		parent_window->m_tabarea.h));

	static float anim_progress = 0.f;
	//static float Blue = 0.4f;
	//static float Red = 0.1f;
	//static bool Drag = false;
	//Blue -= 0.01f;
	//if (Blue < 0.f)
	//	Blue += 1.f;
	//Red -= 0.01f;
	//if (Red < 0.f)
	//	Red += 1.f;

	if (selected)
	{
		render::get().text(Vector2D(parent_window->m_tabarea.x + 110 * m_id + add,
			parent_window->m_tabarea.y - 10),
			name, Color(215, 240, 250, 250), fonts::tab_new_font, c_font::centered_x);

		anim_progress++;
	}
	else
	{
		//if (hovered)
		//{
		//	render::get().text(Drag ? Vector2D((parent_window->m_tabarea.x + 2) + 90 * m_id + add, parent_window->m_tabarea.y - 23) :
		//								Vector2D((parent_window->m_tabarea.x - 2) + 90 * m_id + add, parent_window->m_tabarea.y - 19),
		//		name, Color(10, 10, 255, Blue * 255.f), fonts::tabfontthicc, c_font::left_aligned);

		//	render::get().text(Drag ? Vector2D((parent_window->m_tabarea.x - 2) + 90 * m_id + add, parent_window->m_tabarea.y - 23) :
		//								Vector2D((parent_window->m_tabarea.x + 2) + 90 * m_id + add, parent_window->m_tabarea.y - 19),
		//		name, Color(255, 10, 10, Red * 255.f), fonts::tabfontthicc, c_font::left_aligned);
		//}

		render::get().text(Vector2D(parent_window->m_tabarea.x + 110 * m_id + add,
			parent_window->m_tabarea.y - 10),
			name, Color(210, 220, 205, 250), fonts::tab_new_font, c_font::centered_x);

		anim_progress++;
	}

	//Drag = !Drag;
	//render::get().filled_box((parent_window->m_tabarea.x + 90 * m_id + add) * std::clamp(anim_progress, 0.f, 1.f),
	//	parent_window->m_tabarea.y - 21 - (render::get().get_text_size(name, fonts::tabfontthicc).y / 2),
	//	parent_window->m_tabarea.w,
	//	2, Color(255, 10, 10, 255));

	if (selected)
	{
		for (auto tab : m_subtabs)
		{
			if (tab->update())
				m_selected_subtab = tab;

			tab->render((m_selected_subtab == tab));
		}
	}

	/*bool hovered = c_menu::get().is_hovering(bounds(parent_window->m_tabarea.x + 1, 
														parent_window->m_tabarea.y + 32 + 45 * m_id + add, 
															parent_window->m_tabarea.w - 1, 
																45));

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
	}*/
}

bool c_tab::update()
{
	return c_menu::get().is_pressed(bounds(parent_window->m_tabarea.x + 110 * m_id + add, 
		parent_window->m_tabarea.y - 10,
		parent_window->m_tabarea.w, 
		parent_window->m_tabarea.h));
	
	//return c_menu::get().is_pressed(bounds(parent_window->m_tabarea.x + 1,
	//	parent_window->m_tabarea.y + 32 + 45 * m_id + add, parent_window->m_tabarea.w - 1, 45));
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