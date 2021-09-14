#pragma once

class autostrafer : public singleton<autostrafer>
{
public:
	static void strafe();
    QAngle	    m_strafe_angles;
};
