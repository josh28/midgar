#include "mgpch.h"
#include "Application.h"
#include "Input.h"

#include <glad/glad.h>

namespace Midgar
{
	Application* Application::Instance = nullptr;

	Application::Application()
	{
		MG_CORE_ASSERT(!Instance, "Application already exists!");
		Instance = this;

		imGuiLayer = new ImGuiLayer();

		window = std::unique_ptr<Window>(Window::Create());
		window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		PushOverlay(imGuiLayer);
	}

	Application::~Application()
	{
		for (auto it = layerStack.end(); it != layerStack.begin(); )
		{
			(*--it)->OnDetach();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = layerStack.end(); it != layerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	void Application::Run()
	{
		while (isRunning)
		{
			glClearColor(0.5, 0.5, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : layerStack)
				layer->OnUpdate();

			RenderImGuiLayers();
			window->OnUpdate();
		}
	}

	void Application::RenderImGuiLayers()
	{
		imGuiLayer->Begin();

		for (Layer* layer : layerStack)
			layer->OnImGuiRender();

		imGuiLayer->End();
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		isRunning = false;
		return true;
	}
}