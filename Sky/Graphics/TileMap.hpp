#ifndef SKY_TILEMAP_HPP
#define SKY_TILEMAP_HPP

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#ifndef SKY_COMPILE_PUGIXML
#define PUGIXML_HEADER_ONLY
#include "pugixml.cpp"
#endif

#include "../libs/pugixml.hpp"

namespace sky {
using PropertyMap = std::unordered_map<std::string, std::string>;

struct Object {
    PropertyMap properties;
    std::vector<sf::Vector2i> points;
    sf::FloatRect rect;
    std::string name = "None";
    std::string type = "None";
    unsigned gid = 0;
    bool visible = false;
    Object() = default;

    template<typename T>
    T getPropertyAs(std::string name) {
        std::istringstream ss(std::move(name));
        T result;
        ss >> result;
        return result;
    }

    std::string getProperty(std::string name) {
        return properties.at(std::move(name));
    }
};

struct Layer {
    sf::VertexArray vertices;
    std::vector<unsigned> tiles;
    std::string name = "untitled";
    unsigned width = 0;
    unsigned height = 0;
    unsigned char opacity = 255;
    Layer() = default;
    void update(unsigned tileWidth, unsigned tileHeight, unsigned firstgid, sf::Vector2u tileInfo) {
        vertices.setPrimitiveType(sf::Quads);
        vertices.resize(width * height * 4);

        for(unsigned i = 0; i < width; ++i) {
            for(unsigned j = 0; j < height; ++j) {
                unsigned gid = tiles[i + j * width];
                sf::Vertex* quad = &vertices[(i + j * width) * 4];

                if(gid != 0) {
                    gid -= firstgid;
                }
                else {
                    quad[0].color = sf::Color::Transparent;
                    quad[1].color = sf::Color::Transparent;
                    quad[2].color = sf::Color::Transparent;
                    quad[3].color = sf::Color::Transparent;
                }

                unsigned mod = gid % (tileInfo.x / tileWidth);
                unsigned div = gid / (tileInfo.x / tileWidth);

                quad[0].position = sf::Vector2f(i * tileWidth, j * tileHeight);
                quad[1].position = sf::Vector2f((i + 1) * tileWidth, j * tileHeight);
                quad[2].position = sf::Vector2f((i + 1) * tileWidth, (j + 1) * tileHeight);
                quad[3].position = sf::Vector2f(i * tileWidth, (j + 1) * tileHeight);

                quad[0].texCoords = sf::Vector2f(mod * tileWidth, div * tileHeight);
                quad[1].texCoords = sf::Vector2f((mod + 1) * tileWidth, div * tileHeight);
                quad[2].texCoords = sf::Vector2f((mod + 1) * tileWidth, (div + 1) * tileHeight);
                quad[3].texCoords = sf::Vector2f(mod * tileWidth, (div + 1) * tileHeight);
            }
        }
    }
};

struct Tile {
    unsigned id = 0;
    PropertyMap properties;
};

class TileMap : public sf::Drawable, public sf::Transformable {
private:
    std::vector<sf::FloatRect> solidObjects;
    std::vector<Layer> layers;
    std::vector<Object> objects;
    std::vector<Tile> tiles;
    sf::Texture spritesheet;
    unsigned width = 0;
    unsigned height = 0;
    unsigned tileWidth = 0;
    unsigned tileHeight = 0;
    unsigned spacing = 0;
    unsigned margin = 0;
    unsigned firstTileID = 1;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        states.texture = &spritesheet;

        for(auto && layer : layers) {
            target.draw(layer.vertices, states);
        }
    }
public:
    TileMap() = default;

    void clear() {
        solidObjects.clear();
        layers.clear();
        objects.clear();
    }

    bool loadFromTMX(const std::string& filename) {
        clear();

        pugi::xml_document file;
        pugi::xml_parse_result result = file.load_file(filename.c_str());

        if(!result) {
            return result;
        }

        pugi::xml_node map = file.child("map");
        std::string orientation = map.attribute("orientation").as_string();

        if(orientation != "orthogonal") {
            return false; // Non-orthogonal maps not supported
        }

        // Map info
        width = map.attribute("width").as_uint();
        height = map.attribute("height").as_uint();
        tileWidth = map.attribute("tilewidth").as_uint();
        tileHeight = map.attribute("tileheight").as_uint();

        auto children = map.child("tileset");
        firstTileID = children.attribute("firstgid").as_uint();
        spacing = children.attribute("spacing").as_uint();
        margin = children.attribute("margin").as_uint();

        auto imagenode = children.child("image");
        std::string imagePath = imagenode.attribute("source").as_string();

        if(!spritesheet.loadFromFile(imagePath)) {
            return false;
        }

        // Tile info
        for(auto node = children.child("tile"); node; node.next_sibling("tile")) {
            Tile tile;
            tile.id = node.attribute("id").as_uint();

            auto properties = node.child("properties");

            if(properties) {
                for(auto property = properties.child("property"); property; property = property.next_sibling()) {
                    tile.properties.emplace(property.attribute("name").as_string(),
                                            property.attribute("value").as_string());
                }
            }

            tiles.emplace_back(std::move(tile));
        }

        // Initialize layers

        for(auto node = map.child("layer"); node; node = node.next_sibling("layer")) {
            Layer layer;
            bool visibility = node.attribute("visible").as_bool(true);

            if(visibility) {
                float opacity = node.attribute("opacity").as_float();

                if(opacity != 0.f) {
                    layer.opacity = static_cast<unsigned char>(opacity * 255.f);
                }
                else {
                    layer.opacity = 255;
                }
            }
            else {
                layer.opacity = 0;
            }

            layer.name = node.attribute("name").as_string();
            layer.width = node.attribute("width").as_uint();
            layer.height = node.attribute("height").as_uint();

            auto tiledata = node.child("data");
            std::string data = tiledata.text().as_string();
            std::stringstream ss(data);
            unsigned id;

            while(ss >> id) {
                layer.tiles.push_back(id);

                if(ss.peek() == ',') {
                    ss.ignore();
                }
            }

            layer.update(tileWidth, tileHeight, firstTileID, spritesheet.getSize());
            layers.push_back(layer);
        }

        // Initialize objects
        for(auto node = map.child("objectgroup"); node; node = node.next_sibling("objectgroup")) {
            for(auto element = node.child("object"); element; element = element.next_sibling("object")) {
                Object object;
                sf::FloatRect objrect;
                object.type = element.attribute("type").as_string();
                object.name = element.attribute("name").as_string();
                objrect.left = element.attribute("x").as_float();
                objrect.top = element.attribute("y").as_float();
                objrect.width = element.attribute("width").as_float();
                objrect.height = element.attribute("height").as_float();
                object.gid = element.attribute("gid").as_int();
                object.visible = element.attribute("visible").as_bool(true);

                if(object.type == "solid") {
                    solidObjects.push_back(objrect);
                }

                object.rect = objrect;

                auto properties = element.child("properties");

                if(properties) {
                    for(auto property = properties.child("property"); property; property = property.next_sibling()) {
                        std::string name = property.attribute("name").as_string();
                        std::string value = property.attribute("value").as_string();

                        if(!name.empty()) {
                            object.properties[name] = value;
                        }
                    }
                }

                auto poly = element.child("polygon");

                if(!poly) {
                    poly = element.child("polyline");
                }

                if(poly) {
                    std::string pointlist = poly.attribute("points").as_string();
                    std::stringstream ss(pointlist);
                    std::vector<std::string> points;
                    std::string str;

                    while(std::getline(ss, str, ' ')) {
                        points.push_back(str);
                    }

                    for(auto && point : points) {
                        std::vector<unsigned> coords;
                        std::stringstream pt(point);
                        unsigned coord;

                        while(pt >> coord) {
                            coords.push_back(coord);

                            if(pt.peek() == ',') {
                                pt.ignore();
                            }
                        }

                        object.points.push_back(sf::Vector2i(coords[0], coords[1]));
                    }
                }

                objects.push_back(object);
            }
        }

        return true;
    }

    template<typename Predicate>
    std::vector<Object> getObjects(Predicate pred) const {
        std::vector<Object> result;
        for(auto&& obj : objects) {
            if(pred(obj)) {
                result.emplace_back(std::move(obj));
            }
        }
        return result;
    }

    std::vector<Object> getObjects() const {
        return objects;
    }

    template<typename Predicate>
    std::vector<Tile> getTiles(Predicate pred) const {
        std::vector<Tile> result;
        for(auto&& tile : tiles) {
            if(pred(tile)) {
                result.emplace_back(std::move(tile));
            }
        }
        return result;
    }

    std::vector<Tile> getTiles() const {
        return tiles;
    }

    bool tileHasProperty(unsigned id, const std::string& str) const {
        auto predicate = [&](const Tile& tile) {
            return tile.id == id && tile.properties.count(str);
        };

        for(auto&& tile : tiles) {
            if(predicate(tile)) {
                return true;
            }
        }

        return false;
    }
};
} // sky

#endif // SKY_TILEMAP_HPP
