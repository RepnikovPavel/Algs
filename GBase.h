#pragma once
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <string>
#include <iostream>

template<
	typename _VertexNameType,
	typename _VertexType,
	typename _EdgeDataType
>
class Graph final
{
	class _ProxyName;
public:
	void InsertVertex(
		const _VertexNameType& vertex_name,const _VertexType& vertex,
		const std::map<_VertexNameType, _EdgeDataType>& adj_list_and_edges_data
	)
	{
		if (DoesItPhysicallyExist(vertex_name)==false)
		{

			int unique_id = _proxy_name.GetUID(vertex_name);
			_vertexes[unique_id] = vertex;
			_names_of_vertexes.push_back(unique_id);
			//_adjacency_list[unique_id] = {};
			//_edges[unique_id] = {};

			std::vector<int> unique_ids(adj_list_and_edges_data.size());
			size_t i = 0;
			for (auto it = adj_list_and_edges_data.cbegin(); it != adj_list_and_edges_data.cend(); it++)
			{
				unique_ids[i] = _proxy_name.GetUID(it->first);
				_virtual_edges[unique_id][unique_ids[i]] = it->second;
				i++;
			}
			_virtual_adjacency_list[unique_id] = unique_ids;
			

			MaintainConsistencyAfterInsert(unique_id);
		}
	}
	void DeleteVertex(const _VertexNameType& vertex_name)
	{
		if (DoesItPhysicallyExist(vertex_name))
		{
			int unique_id = _proxy_name.GetUID(vertex_name);
			_vertexes.erase(unique_id);
			_adjacency_list.erase(unique_id);
			_edges.erase(unique_id);
			_proxy_name.DeleteName(vertex_name);
			auto pos = std::find(_names_of_vertexes.begin(), _names_of_vertexes.end(), unique_id);
			if (pos!=_names_of_vertexes.end())
			{
				_names_of_vertexes.erase(pos);
			}

			MaintainConsistencyAfterDelete(unique_id);
		}
	}

	void SendAdjListToOSTREAM() 
	{
		std::cout << "adjacency list:" << std::endl;
		for (auto it = _adjacency_list.cbegin(); it != _adjacency_list.cend(); it++)
		{
			std::string output = "\t[";
			output += _proxy_name.GetNameByUID(it->first);
			output += "]->[";
			for (auto list_it = it->second.cbegin(); list_it != it->second.cend(); list_it++)
			{
				output += _proxy_name.GetNameByUID(*list_it);
				output += ' ';
			}
			output += ']';
			std::cout << output << std::endl;
		}
	}
	void SendVertexesInfoToOSTREAM()
	{
		std::cout << "vertexes:" << std::endl;
		for (auto it = _vertexes.begin(); it != _vertexes.end(); it++)
		{
			int vertex_uid = it->first;
			std::string output = "\tvertex ";
			output += _proxy_name.GetNameByUID(vertex_uid);
			output += " data ";
			output += it->second.get_data_as_str();
			std::cout << output << std::endl;
		}
	}
	void SendEdgesInfoToOSTREAM()
	{
		std::cout << "edges:" << std::endl;
		for (auto it = _edges.begin(); it != _edges.end(); it++)
		{
			int vertex_uid = it->first;
			auto map_with_edges_to_other_vertexes = it->second;
			if (map_with_edges_to_other_vertexes.size() != 0)
			{
				for (auto edge_it = map_with_edges_to_other_vertexes.begin(); edge_it != map_with_edges_to_other_vertexes.end(); edge_it++)
				{
					std::string output = "\tedge FROM ";
					output += _proxy_name.GetNameByUID(vertex_uid);
					output += " TO ";
					output += _proxy_name.GetNameByUID(edge_it->first);
					output += " data ";
					output += edge_it->second.get_data_as_str();
					std::cout << output << std::endl;

				}
			}
		}
	}
	

private:



	bool DoesItPhysicallyExist(const _VertexNameType& vertex_name)
	{
		if (_proxy_name.IsExists(vertex_name))
		{
			int name_to_check = _proxy_name.GetUID(vertex_name);
			auto pos = std::find(_names_of_vertexes.begin(), _names_of_vertexes.end(), name_to_check);
			if (pos == _names_of_vertexes.end())
			{
				return false;
			}
			else 
			{
				return true;
			}
		}
		else { return false; }
	}

	void MaintainConsistencyAfterInsert(int unique_id)
	{

		const auto& v_adj_list = _virtual_adjacency_list[unique_id];
		const auto& v_edg_list = _virtual_edges[unique_id];

		for(auto it = v_adj_list.cbegin();it!=v_adj_list.cend();it++)
		{
			int virtual_name = *it;
			auto pos = std::find(_names_of_vertexes.cbegin(), _names_of_vertexes.cend(), virtual_name);
			if (pos != _names_of_vertexes.cend())
			{
				_adjacency_list[unique_id].push_back(virtual_name);
				_edges[unique_id][virtual_name] = _virtual_edges[unique_id][virtual_name];
			}
		} 
		//добавили имена вершин в список смещности, если вершины, им соотв.-е суще.-ют. и добавили данные ребер аналогично.
		
		for (auto _v_adj_lists_it = _virtual_adjacency_list.cbegin(); _v_adj_lists_it != _virtual_adjacency_list.cend(); _v_adj_lists_it++)
		{
			auto& other_v_adj_list = _v_adj_lists_it->second;
			auto pos = std::find(other_v_adj_list.begin(), other_v_adj_list.end(), unique_id);
			if (pos != other_v_adj_list.end())
			{
				_adjacency_list[_v_adj_lists_it->first].push_back(unique_id);
				_edges[_v_adj_lists_it->first][unique_id] = _virtual_edges[_v_adj_lists_it->first][unique_id];
			}
		}
		// добавили ссылку на вставленную вершину в реальный список смежности всех остальных вершин, если там эти ссылки были вирт.-ыми

	}

	void MaintainConsistencyAfterDelete(int unique_id)
	{
		
		_adjacency_list.erase(unique_id);
		_edges.erase(unique_id);
		
		for (auto it = _adjacency_list.begin(); it != _adjacency_list.end(); it++)
		{
			auto& adj_list = it->second;
			auto pos = std::find(adj_list.begin(), adj_list.end(), unique_id);
			if (pos != adj_list.end())
			{
				adj_list.erase(pos);
			}
			
		}
		for (auto it = _edges.begin(); it != _edges.end(); it++)
		{
			auto& edge_map = it->second;
			edge_map.erase(unique_id);
		}

		_virtual_adjacency_list.erase(unique_id);
		_virtual_edges.erase(unique_id);
		for (auto it = _virtual_adjacency_list.begin(); it != _virtual_adjacency_list.end(); it++)
		{
			auto& adj_list = it->second;
			auto pos = std::find(adj_list.begin(), adj_list.end(), unique_id);
			
			if (pos != adj_list.end())
			{
				adj_list.erase(pos);
			}
			
		}
		for (auto it = _virtual_edges.begin(); it != _virtual_edges.end(); it++)
		{
			auto& edge_map = it->second;
			edge_map.erase(unique_id);
		}
	}

	std::map<int,_VertexType> _vertexes;
	std::map<int,std::map<int, _EdgeDataType>> _edges;
	std::map<int,std::vector<int>> _adjacency_list;
	std::list<int> _names_of_vertexes;

	std::map<int, std::map<int, _EdgeDataType>> _virtual_edges;
	std::map<int, std::vector<int>> _virtual_adjacency_list;

	_ProxyName _proxy_name;
	
private:
	class _ProxyName
	{
	public:

		bool IsExists(const _VertexNameType& vertex_name) const
		{
			if (_vertex_id_map.find(vertex_name) == _vertex_id_map.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		int GetUID(const _VertexNameType& vertex_name)
		{
			bool is_exists = IsExists(vertex_name);
			if (is_exists==false)
			{
				_count++;
				_vertex_id_map[vertex_name] = _count;
				_vertex_name_map[_count] = vertex_name;
				return _count;
			}
			else
			{
				return _vertex_id_map[vertex_name];
			}
		}
		const _VertexNameType& GetNameByUID(int vertex_id) 
		{
			return _vertex_name_map[vertex_id];
		}
		void DeleteName(const _VertexNameType& vertex_name)
		{
			_vertex_name_map.erase(GetUID(vertex_name));
			_vertex_id_map.erase(vertex_name);
		}

	private:

		int _count = 0;
		std::map<_VertexNameType, int> _vertex_id_map;
		std::map<int, _VertexNameType> _vertex_name_map;
	};
};

