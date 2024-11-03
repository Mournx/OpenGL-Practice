#pragma once

extern int WINDOW_WIDTH, WINDOW_HEIGHT;
class ImGuiLayer
{
public:
	ImGuiLayer();
	~ImGuiLayer();

	void OnAttach();
	void OnDetach();

	void Begin();
	void End();
private:
	float m_Time = 0;
};
