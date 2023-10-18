#include <SFML/Graphics.hpp>
#include <blend2d.h>

#include <iostream>

int main()
{
  const char* font_file_name = "OpenSans-Regular.ttf";
  float screen_w = 800.f, screen_h = 600.f;

  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
  window.setVerticalSyncEnabled(true);

  sf::Image sf_image;
  sf_image.create(800, 600, sf::Color::Black);

  BLImage bl_image;
  bl_image.createFromData(800, 600, BL_FORMAT_PRGB32, (void*)sf_image.getPixelsPtr(), 4*800);

  BLContextCreateInfo ctx_info;
  ctx_info.threadCount = 2;

  BLFontFace font_face;
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
      return 1;
    }
  }
  else
  {
    std::cerr << "failed to open font file " << font_file_name << std::endl;
    return 1;
  }

  {
    BLContext ctx(bl_image, ctx_info);
    ctx.setHint(BL_CONTEXT_HINT_RENDERING_QUALITY, BL_RENDERING_QUALITY_ANTIALIAS);
    ctx.setCompOp(BL_COMP_OP_PLUS);

    BLFont font;
    float y = 20.f, font_size = 24.f;
    const char* str = "The quick brown fox jumped over your mom.";
    const std::size_t str_len = std::strlen(str);

    while(y < screen_h)
    {
      font.createFromFace(font_face, font_size);
      ctx.fillUtf8Text(BLPoint(10, y + font_size / 2.f), font, str, strlen(str), BLRgba32(0xF0F0F0FFu));

      y += font_size + 4.f;
      font_size += 12.f;
    }
  }

  sf::Texture texture;
  texture.loadFromImage(sf_image);

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

      default:
        break;
      }
    }

    window.clear();

    sf::Sprite sprite;
    sprite.setTexture(texture);
    window.draw(sprite);

    window.display();
  }

  return 0;
}
