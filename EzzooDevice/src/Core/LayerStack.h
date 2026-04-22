#pragma once
#include "Layer.h"

namespace Ezzoo {


	class LayerStack 
	{
	public :

		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverLay(Layer* Layer);

		void PopLayer(Layer* layer);
		void PopOverLay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

		std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

		std::vector<Layer*>::const_iterator cbegin() { return m_Layers.cbegin(); }
		std::vector<Layer*>::const_iterator cend() { return m_Layers.cend(); }

	private :
		std::vector<Layer*>  m_Layers;
		unsigned int m_LayerInsertIndex{0};
	};
}



