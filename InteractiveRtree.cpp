//
// Created by msumi on 23/11/2024.
//

#include "InteractiveRtree.hpp"

void InteractiveRtree::displayObjects(std::vector<std::list<object_type>::iterator> overlappingObjects) {
	for (auto const& obj : objects) {
		window.draw(obj);
	}
	for (auto const& obj : overlappingObjects) {
		sf::RectangleShape newObj = *obj;
		newObj.setFillColor(sf::Color::Blue);
		window.draw(newObj);
	}
}

InteractiveRtree::InteractiveRtree(){
	window.create(sf::VideoMode(1280, 720), "Interactive Rtree");
	window.setFramerateLimit(60);
}

InteractiveRtree::~InteractiveRtree() = default;

void InteractiveRtree::run() {
	sf::Vector2f startPoint;
	bool isDrawing = false;
	float searchRangeLenght = 0.0f;
	std::vector<std::list<object_type>::iterator> windowQueryResult;

	while (window.isOpen()) {
		point_type searchRangeMin = {window.mapPixelToCoords(sf::Mouse::getPosition(window)).x - searchRangeLenght,
								 window.mapPixelToCoords(sf::Mouse::getPosition(window)).y - searchRangeLenght};
		point_type searchRangeMax = {window.mapPixelToCoords(sf::Mouse::getPosition(window)).x + searchRangeLenght,
									 window.mapPixelToCoords(sf::Mouse::getPosition(window)).y + searchRangeLenght};
		aabb_type searchRange(searchRangeMin, searchRangeMax);
		rtree.search_overlap(searchRange,
												 [&](rtree_type::value_type v)
												 {
													 windowQueryResult.push_back(v.second);
													 return false;
												 });
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left) {
					startPoint = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					isDrawing = true;
				}
				else if(event.mouseButton.button == sf::Mouse::Right) {
					for(auto& obj : windowQueryResult) {
						point_type minPoint = {obj->getPosition().x, obj->getPosition().y};
						point_type maxPoint = {obj->getPosition().x + obj->getSize().x, obj->getPosition().y + obj->getSize().y};
						aabb_type boundingBox(minPoint, maxPoint);
						std::pair<aabb_type, std::list<object_type>::iterator> entrie = {boundingBox, obj};
						rtree.deleteEntrie(entrie);
						objects.erase(obj);
					}
					windowQueryResult.clear();
				}
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left && isDrawing) {
					sf::Vector2f endPoint = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					isDrawing = false;

					point_type minPoint = { std::min(startPoint.x, endPoint.x), std::min(startPoint.y, endPoint.y)};
					point_type maxPoint = { std::max(startPoint.x, endPoint.x), std::max(startPoint.y, endPoint.y)};
					aabb_type boundingBox(minPoint, maxPoint);

					sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(maxPoint[0] - minPoint[0], maxPoint[1] - minPoint[1]));
					rect.setPosition(minPoint[0], minPoint[1]);
					rect.setFillColor(sf::Color::Black);
					objects.push_back(rect);
					auto it = prev(objects.end());

					rtree.insert({boundingBox, it});
				}
			}
			else if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
					float delta = event.mouseWheelScroll.delta;
					if (delta > 0) {
						searchRangeLenght += 10.0f;
					}
					else {
						searchRangeLenght = std::max(0.0f, searchRangeLenght - 10.0f);
					}
				}
			}
		}
		window.clear(sf::Color::White);
		displayObjects(windowQueryResult);
		displayRtreeAABBs(rtree.root()->as_node(), 0);
		if(isDrawing) {
			sf::Vector2f currentPoint = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			sf::VertexArray line(sf::LinesStrip, 2);
			line[0].position = startPoint;
			line[1].position = currentPoint;
			line[0].color = sf::Color::Black;
			line[1].color = sf::Color::Black;
			window.draw(line);
		}
		// Draw search range
		sf::RectangleShape searchRect(sf::Vector2f(searchRangeMax[0] - searchRangeMin[0], searchRangeMax[1] - searchRangeMin[1]));
		searchRect.setPosition(searchRangeMin[0], searchRangeMin[1]);
		searchRect.setFillColor(sf::Color::Transparent);
		searchRect.setOutlineColor(sf::Color::Black);
		searchRect.setOutlineThickness(2);
		window.draw(searchRect);
		window.display();

		windowQueryResult.clear();
	}
}
