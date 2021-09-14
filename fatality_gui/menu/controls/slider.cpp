#include "../external_include.h"

c_slider::c_slider(std::string name, bounds pbounds, std::string option, float min, float max, c_subtab* parent, std::string format)
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_parent = parent;
	this->m_format = format;
	this->m_max = max;
	this->m_min = min;
	this->m_roundvalue = false;

	_u(key, m_value);
	g_cfg[key]->set<float>(min);
}

c_slider::c_slider(std::string name, bounds pbounds, std::string option, int min, int max, c_subtab* parent, std::string format)
{
	this->m_name = name;
	this->m_bounds = pbounds;
	this->m_value = option;
	this->m_parent = parent;
	this->m_format = format;
	this->m_max = max;
	this->m_min = min;
	this->m_roundvalue = true;

	_u(key, m_value);
	g_cfg[key]->set<int>(min);
}

void c_slider::render()
{
	char buffer[24];
	_u(key, m_value);
	_u(format_s, m_format);
	if (m_roundvalue)
		sprintf_s(buffer, format_s, g_cfg[key]->get<int>());
	else
		sprintf_s(buffer, format_s, g_cfg[key]->get<float>());
	std::string format(buffer);

	float slider_ratio = (g_cfg[key]->get<float>() - m_min) / (m_max - m_min);
	float slider_location = slider_ratio * SLIDER_WIDTH;

	render::get().rect(Vector2D(m_bounds.x, m_bounds.y + 6), Vector2D(SLIDER_WIDTH, SLIDER_HEIGHT - 11), c_menu::get().is_pressed(bounds(m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT))
		|| m_active || c_menu::get().is_pressed(bounds(slider_location - 8, m_bounds.y + (SLIDER_HEIGHT / 2 + 1) - 8, 16, 16)) ? D3DCOLOR_RGBA(235, 5, 90, 250) : m_hovered ? D3DCOLOR_RGBA(154, 10, 79, 250) : D3DCOLOR_RGBA(60, 53, 93, 250));

	_u(name, m_name);

	render::get().text(Vector2D(m_bounds.x + SLIDER_WIDTH + 12,
		m_bounds.y + SLIDER_HEIGHT / 2 + 1 - (render::get().get_text_size(name, fonts::controlfont).y / 2)),
		name, Color(180, 180, 187, 250), fonts::controlfont);

	render::get().text(Vector2D(m_bounds.x + SLIDER_WIDTH + 16 + render::get().get_text_size(name, fonts::controlfont).x,
		m_bounds.y + SLIDER_HEIGHT / 2 + 1 - (render::get().get_text_size(name, fonts::controlfont).y / 2)),
		format, Color(167, 169, 180, 250), fonts::controlfont);

	render::get().rect_filled(Vector2D(m_bounds.x, m_bounds.y + 6), Vector2D(slider_location, SLIDER_HEIGHT - 11), c_menu::get().is_pressed(bounds(m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT))
		|| m_active || c_menu::get().is_pressed(bounds(slider_location - 8, m_bounds.y + (SLIDER_HEIGHT / 2 + 1) - 8, 16, 16)) ? D3DCOLOR_RGBA(235, 5, 90, 250) : m_hovered ? D3DCOLOR_RGBA(154, 10, 79, 250) : D3DCOLOR_RGBA(60, 53, 93, 250));

	render::get().circle_filled(Vector2D(m_bounds.x + slider_location, m_bounds.y + SLIDER_HEIGHT / 2 + 1), 8, c_menu::get().is_pressed(bounds(m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT))
		|| m_active || c_menu::get().is_pressed(bounds(slider_location - 8, m_bounds.y + (SLIDER_HEIGHT / 2 + 1) - 8, 16, 16)) ? D3DCOLOR_RGBA(235, 5, 90, 250) : m_hovered ? D3DCOLOR_RGBA(154, 10, 79, 250) : D3DCOLOR_RGBA(60, 53, 93, 250));
	/*render::get().rect_filled_diamond_gradient(Vector2D(m_bounds.x, m_bounds.y + 6), Vector2D(SLIDER_WIDTH, SLIDER_HEIGHT - 11), D3DCOLOR_RGBA(33, 27, 70, 250), D3DCOLOR_RGBA(235, 5, 90, 250));
	_u(name, m_name);

	render::get().text(Vector2D(m_bounds.x + SLIDER_WIDTH + 12,
		m_bounds.y + SLIDER_HEIGHT / 2 - (render::get().get_text_size(name, fonts::controlfont).y / 2)),
		name, Color(33, 31, 36, 250), fonts::controlfont);

	char buffer[24];
	_u(key, m_value);
	_u(format_s, m_format);
	if (m_roundvalue)
		sprintf_s(buffer, format_s, g_cfg[key]->get<int>());
	else
		sprintf_s(buffer, format_s, g_cfg[key]->get<float>());
	std::string format(buffer);

	render::get().text(Vector2D(m_bounds.x + SLIDER_WIDTH + 16 + render::get().get_text_size(name, fonts::controlfont).x,
		m_bounds.y + SLIDER_HEIGHT / 2 - (render::get().get_text_size(name, fonts::controlfont).y / 2)),
		format, Color(33, 31, 36, 250), fonts::controlfont);

	float slider_ratio = (g_cfg[key]->get<float>() - m_min) / (m_max - m_min);
	float slider_location = slider_ratio * SLIDER_WIDTH;

	render::get().circle_filled(Vector2D(m_bounds.x + slider_location, m_bounds.y + SLIDER_HEIGHT / 2), 5, Color(255, 255, 255, 255));
	render::get().circle(Vector2D(m_bounds.x + slider_location, m_bounds.y + SLIDER_HEIGHT / 2), 5, Color(255, 10, 10, 255));*/
}

void c_slider::update()
{
	_u(key, m_value);
	float slider_ratio = (g_cfg[key]->get<float>() - m_min) / (m_max - m_min);
	float slider_location = slider_ratio * SLIDER_WIDTH;

	if (!c_menu::get().is_in_area(c_menu::get().get_press_pos(), bounds(m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT))
		&& !c_menu::get().is_in_area(c_menu::get().get_press_pos(), bounds(slider_location - 8, m_bounds.y + (SLIDER_HEIGHT / 2) - 8, 16, 16)))
		return;

	if (c_menu::get().is_pressed(bounds(m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT))
		|| c_menu::get().is_pressed(bounds(slider_location - 8, m_bounds.y + (SLIDER_HEIGHT / 2) - 8, 16, 16)))
	{
		float new_pos, slider_ratio;
		new_pos = (float)(c_menu::get().get_mouse_pos().x - m_bounds.x - 1);

		if (new_pos < 0)
			new_pos = 0;

		if (new_pos > SLIDER_WIDTH)
			new_pos = (int)SLIDER_WIDTH;

		slider_ratio = new_pos / (float)(SLIDER_WIDTH);
		g_cfg[key]->set(m_min + (m_max - m_min) * slider_ratio);
	}
	else if (c_menu::get().is_holding(bounds(m_bounds.x, m_bounds.y, SLIDER_WIDTH, SLIDER_HEIGHT))
		|| c_menu::get().is_holding(bounds(slider_location - 8, m_bounds.y + (SLIDER_HEIGHT / 2) - 8, 16, 16)))
		m_active = true;

	if (m_active)
	{
		if (!c_menu::get().mouse_is_holding())
			m_active = false;

		m_wasactive = true;

		m_parent->focussed_control = this;

		float new_pos, slider_ratio;
		new_pos = (float)(c_menu::get().get_mouse_pos().x - m_bounds.x - 1);

		if (new_pos < 0)
			new_pos = 0;

		if (new_pos > SLIDER_WIDTH)
			new_pos = (int)SLIDER_WIDTH;

		slider_ratio = new_pos / (float)(SLIDER_WIDTH);
		g_cfg[key]->set(m_min + (m_max - m_min) * slider_ratio);
	}
	else
	{
		if (m_roundvalue && m_wasactive)
		{
			g_cfg[key]->set(round_value());
			m_wasactive = false;
		}
		m_parent->focussed_control = nullptr;
	}
}

float c_slider::round_value()
{
	_u(key, m_value);
	int rounded = g_cfg[key]->get<float>();
	return rounded;
}

void c_slider::update_pos_individual(LONG xdrag, LONG ydrag)
{

}