#ifndef INTERACTIVERTREE_HPP
#define INTERACTIVERTREE_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>

#include "rtree/AABB.hpp"
#include "rtree/RTree.hpp"

class InteractiveRtree {
	using object_type = sf::RectangleShape;
	using point_type = rtree::point_t<float, 2>;
	using aabb_type = rtree::aabb_t<point_type>;
	using rtree_type = rtree::RTree<aabb_type, aabb_type, std::list<object_type>::iterator, 2, 4>;

	rtree_type rtree;
	std::list<object_type> objects;
	sf::RenderWindow window;
	sf::Event event{};

	template <typename NodeType>
	void displayRtreeAABBs(NodeType node, int level = 0) {
		if(level != rtree.leaf_level()) {
			for(rtree::size_type i = 0; i < node->size(); ++i) {
				displayRtreeAABBs(node->at(i).second->as_node(), level + 1);
			}
		}
		for (rtree::size_type i = 0; i < node->size(); ++i) {
			auto const& aabb = node->at(i).first;
			sf::RectangleShape obj;
			obj.setSize({aabb.max_[0] - aabb.min_[0], aabb.max_[1] - aabb.min_[1]});
			obj.setPosition(aabb.min_[0], aabb.min_[1]);
			obj.setFillColor(sf::Color::Transparent);
			float intensity = (level) / (rtree.leaf_level()+1.0);
			obj.setOutlineColor(sf::Color(255, 255*intensity, 255*intensity));
			obj.setOutlineThickness(5);
			window.draw(obj);
		}
	}
	void displayObjects(std::vector<std::list<object_type>::iterator> overlappingObjects);
public:
	InteractiveRtree();
	~InteractiveRtree();
	void run();
};

#endif //INTERACTIVERTREE_HPP
