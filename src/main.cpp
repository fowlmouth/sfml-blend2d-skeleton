#include <SFML/Graphics.hpp>
#include <blend2d.h>

#include <iostream>

class Application
{
  sf::RenderWindow window;
  sf::Image image;
  sf::Texture texture;
  BLFontFace font_face;
  BLContextCreateInfo bl_context_info;
  bool texture_dirty;

public:

  struct InvalidFont
  {
  };

  Application(unsigned int width, unsigned int height, const char* font_file_name);

  int run();

  void recreate_image();
  void recreate_texture();
};

Application::Application(unsigned int width, unsigned int height, const char* font_file_name)
{
  BLArray<uint8_t> data_buffer;
  if(BLFileSystem::readFile(font_file_name, data_buffer) == BL_SUCCESS)
  {
    BLFontData font_data;
    if(font_data.createFromData(data_buffer) == BL_SUCCESS)
    {
      font_face.createFromData(font_data, 0);
    }
    else
    {
      std::cerr << "failed to load font file " << font_file_name << std::endl;
      throw InvalidFont{};
    }
  }
  else
  {
    std::cerr << "failed to open font file " << font_file_name << std::endl;
    throw InvalidFont{};
  }

  window.create(sf::VideoMode(width, height), "SFML / Blend2D works!");
  window.setVerticalSyncEnabled(true);

  bl_context_info.threadCount = 2;
}

int Application::run()
{
  recreate_image();

  sf::View view = window.getDefaultView();

  while(window.isOpen())
  {
    sf::Event event;
    while(window.pollEvent(event))
    {
      switch(event.type)
      {
      case sf::Event::Closed:
        window.close();
        break;

      case sf::Event::KeyPressed:
        if(event.key.code == sf::Keyboard::Escape)
        {
          window.close();
        }
        break;

      case sf::Event::Resized:
        recreate_image();
        view.setSize(event.size.width, event.size.height);
        view.setCenter(event.size.width / 2.f, event.size.height / 2.f);
        break;

      default:
        break;
      }
    }

    if(texture_dirty)
    {
      recreate_texture();
    }

    window.setView(view);
    window.clear(sf::Color::Black);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    window.draw(sprite);

    window.display();
  }

  return 0;
}

void Application::recreate_image()
{
  sf::Vector2u size = window.getSize();

  image.create(size.x, size.y, sf::Color::Transparent);

  BLImage bl_image;
  bl_image.createFromData(size.x, size.y, BL_FORMAT_PRGB32, (void*)image.getPixelsPtr(), size.x * 4);

  BLContext ctx(bl_image, bl_context_info);

  ctx.setHint(BL_CONTEXT_HINT_RENDERING_QUALITY, BL_RENDERING_QUALITY_ANTIALIAS);
  ctx.setCompOp(BL_COMP_OP_PLUS);

  BLFont font;
  float y = 20.f, font_size = 24.f;
  const char* str = "The quick brown fox jumped over your mom.";
  const std::size_t str_len = std::strlen(str);

  BLRgba32 color{0,0,0,0xFF};
  color.setB(0xFF);
  ctx.setFillStyle(color);

  while(y < (float)size.y)
  {
    font.createFromFace(font_face, font_size);
    ctx.fillUtf8Text(BLPoint(10, y + font_size / 2.f), font, str);

    y += font_size + 4.f;
    font_size += 12.f;
  }

  texture_dirty = true;
}

void Application::recreate_texture()
{
  BLImageData image_data;
  if(!texture.loadFromImage(image))
  {
    return;
  }
  texture_dirty = false;
}

int main()
{
  return Application{800, 600, "OpenSans-Regular.ttf"}.run();
}
