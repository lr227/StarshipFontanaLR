#include "SFApp.h"

SFApp::SFApp(std::shared_ptr<SFWindow> window) : fire(0), is_running(true), sf_window(window) {
  int canvas_w, canvas_h;
  SDL_GetRendererOutputSize(sf_window->getRenderer(), &canvas_w, &canvas_h);

  app_box = make_shared<SFBoundingBox>(Vector2(canvas_w, canvas_h), canvas_w, canvas_h);
  player  = make_shared<SFAsset>(SFASSET_PLAYER, sf_window);
  auto player_pos = Point2((canvas_w/2), 88.0f);
  player->SetPosition(player_pos);

  const int number_of_aliens = 75;
  for(int i=0; i<number_of_aliens; i++) {
    // place an alien at width/number_of_aliens * i
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN, sf_window);
    auto pos   = Point2((canvas_w/number_of_aliens) * i, rand() % 10000 + 550);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  }
  
  //allowing the idea of coin/token collection
  const int number_of_coins = 30;
  for(int k=0; k<number_of_coins; k++) {
    auto coin = make_shared<SFAsset>(SFASSET_COIN, sf_window);
    auto pos  = Point2((canvas_w/number_of_coins) * k, rand() % 10000 + 550);
    coin->SetPosition(pos);
    coins.push_back(coin);
  }
}

SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_UP:
    player->CharacterGoNorth();
    break;
  case SFEVENT_PLAYER_DOWN:
    player->CharacterGoSouth();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->CharacterGoWest();
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->CharacterGoEast();
    break;
  case SFEVENT_FIRE:
    fire ++;
    FireProjectile();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app
  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {
    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}

void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
    p->GoNorth();
  }
  // Update coin position
  for(auto c: coins) {
    c->CoinGoSouth();
  }

  // Update enemy positions
  for(auto a : aliens) {
    a->GoSouth();
  }

  // Detect collisions
  for(auto p : projectiles) {
    for(auto a : aliens) {
      auto projectileposition=p->GetPosition();
      if(p->CollidesWith(a) && projectileposition.getY()<500) {
        p->HandleCollision();
        a->HandleCollision();
      }
    }
  }
  for(auto p : projectiles) {
    for(auto c : coins) {
      int coincount = 0;
      if(p->CollidesWith(c)) {
        c->HandleCollision();
        p->HandleCollision();
        coincount++;
	cout<<coincount;
      }
    }
  }

  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset> > aliens;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      aliens.push_back(a);
    }
  }
  list<shared_ptr<SFAsset> > coins;
  for(auto c : coins) {
    if(c->IsAlive()) {
      coins.push_back(c);
    }
  }
}

void SFApp::OnRender() {
  SDL_RenderClear(sf_window->getRenderer());

  // draw the player
  player->OnRender();

  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender();}
  }

  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender();}
  }

  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender();}
  }

  // Switch the off-screen buffer to be on-screen
  SDL_RenderPresent(sf_window->getRenderer());
}

void SFApp::FireProjectile() {
  auto pb = make_shared<SFAsset>(SFASSET_PROJECTILE, sf_window);
  auto v  = player->GetPosition();
  pb->SetPosition(v);
  projectiles.push_back(pb);
}
