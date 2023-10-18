#include <SFML/Graphics.hpp>
#include <blend2d.h>

int main()
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
  window.setVerticalSyncEnabled(true);

  sf::Image sf_image;
  sf_image.create(800, 600, sf::Color::Black);

  BLImage bl_image;
  bl_image.createFromData(800, 600, BL_FORMAT_PRGB32, (void*)sf_image.getPixelsPtr(), 4*800);

  BLContextCreateInfo ctx_info;
  ctx_info.threadCount = 2;

  {
    BLContext ctx(bl_image, ctx_info);

    BLPath path;
    path.addCircle(BLCircle(400, 300, 100));

    ctx.setCompOp(BL_COMP_OP_PLUS);
    ctx.fillPath(path, BLRgba32(0xAA1199FFu));
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
