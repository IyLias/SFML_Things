#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace std;

#define WIDTH	1000		
#define HEIGHT	600

#define TURN	360

#define MAXNUM		15

#define COLORNUM	7

class GravitySystem {

	// if try to get velocity and pos with total_time, because of sin and cos values are
	// not exactly correct can't get exact pos and velocity 

public:

	sf::Vector2f initVelocity;

	sf::Vector2f initPosition;

	void setInitVelocity(sf::Vector2f vel) {
		initVelocity.x = vel.x, initVelocity.y = vel.y;
	}

	void setInitPosition(sf::Vector2f pos) {
		initPosition.x = pos.x, initPosition.y = pos.y;
	}

	sf::Vector2f getPositionUnderGravity(sf::Time time) {
		return sf::Vector2f((float)-4.9*pow(time.asSeconds(), 2) + initVelocity.x*time.asSeconds() + initPosition.x, (float)-4.9*pow(time.asSeconds(), 2) + initVelocity.y*time.asSeconds() + initPosition.y);
	}

	void setVelocityUnderGravity(sf::Time time,sf::Vector2f& velocity) {

		velocity.y = -9.8 * time.asSeconds() + initVelocity.y;

	}




};

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:

	ParticleSystem(unsigned int count) :
		m_particles(count),
		m_vertices(sf::Points, count),
		m_lifetime(sf::seconds(3.f)),
		m_emitter(0.f, 0.f)
	{
	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);

			// update the position of the corresponding vertex
			m_vertices[i].position += p.velocity * elapsed.asSeconds();

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL;

		// draw the vertex array
		target.draw(m_vertices, states);
	}

private:

	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifetime;
	};

	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
	}

	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};

class Flame : public GravitySystem {

public:

	void setFlame(sf::Vector2f pos, float m_angle, int m_lifetime, float m_speed) {

		float angle = m_angle/180.f * M_PI;
		float speed = m_speed;

		velocity = sf::Vector2f(std::cos(angle)*speed, std::sin(angle)*speed);

		lifetime = sf::seconds(m_lifetime);

		position = pos;

		setInitVelocity(velocity);
		setInitPosition(position);

		//color = sf::Color::Color(sf::Color::Red);
	}


	sf::Color color;

	sf::Vector2f velocity;
	sf::Vector2f position;

	sf::Time lifetime;


};


class FireWork : public sf::Drawable, public sf::Transformable {


public:

	FireWork(unsigned int count) :
		flames(count),
		flame_vertices(sf::Points, count),
		start_vertice(sf::Points, 1)
	{
		startFlame.setFlame(sf::Vector2f((rand() % (WIDTH - 100)) + 10, HEIGHT - 200), 120.f, (rand() % 3) + 5.f, (float)
		(rand() % 40) + 30.f);
		start_vertice[0].position = startFlame.position;
		start_vertice[0].color = colors[rand() % 7];
		startFlameAlive = true;

		m_lifetime = sf::seconds((rand() % 3) + 5.f);

		total_time = sf::Time::Zero;
	}

	void flyingTotheSky(sf::Time elapsed) {

		startFlame.lifetime -= elapsed;
		total_time += elapsed;
		if (startFlame.lifetime <= sf::Time::Zero) {
			startFlameAlive = false;

			for (int i = 0; i < flames.size(); i++) {
				flame_vertices[i].position = start_vertice[0].position;
				flame_vertices[i].color = start_vertice[0].color;
				flames[i].setFlame(flame_vertices[i].position, -(TURN / 2) + (float)i, 7, float(rand() % 25) + 10.f);

			}

			total_time = sf::Time::Zero;

			return;
		}

		startFlame.setVelocityUnderGravity(total_time, startFlame.velocity);
		start_vertice[0].position.x += (elapsed.asSeconds() * startFlame.velocity.x);
		start_vertice[0].position.y -= (elapsed.asSeconds() * startFlame.velocity.y);

		//start_vertice[0].position =  startFlame.position = startFlame.getPositionUnderGravity(total_time);
	}

	void flameExplosion(sf::Time elapsed) {

		total_time += elapsed;

		for (size_t i = 0; i < flames.size(); i++) {

			flames[i].lifetime -= elapsed;
			if (flames[i].lifetime <= sf::Time::Zero) {
				flames[i].lifetime = sf::seconds(0);
			}
			else {
			
				flames[i].setVelocityUnderGravity(total_time, flames[i].velocity);
				flame_vertices[i].position.x += (elapsed.asSeconds() * flames[i].velocity.x);
				flame_vertices[i].position.y -= (elapsed.asSeconds() * flames[i].velocity.y);

				//flame_vertices[i].position = flames[i].getPositionUnderGravity(total_time);
			}

			float ratio = flames[i].lifetime.asSeconds() / m_lifetime.asSeconds();
			flame_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}



	}

	void fireworkShow(sf::Time elapsed) {

		if (startFlame.lifetime > sf::Time::Zero)
			flyingTotheSky(elapsed);
		else
			flameExplosion(elapsed);
	}

	bool getStartFlameAlive() {
		return startFlameAlive;
	}


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform *= getTransform();
		states.texture = NULL;

		if (startFlameAlive == true)
			target.draw(start_vertice, states);
		else
			target.draw(flame_vertices, states);
	}

private:

	Flame startFlame;
	sf::VertexArray start_vertice;
	bool startFlameAlive;

	std::vector<Flame> flames;
	sf::VertexArray flame_vertices;

	sf::Time m_lifetime;

	sf::Time total_time;

	sf::Color colors[COLORNUM] = { sf::Color::Blue,sf::Color::Cyan,sf::Color::Green,sf::Color::Magenta,sf::Color::Red,sf::Color::Yellow,sf::Color::White };

};


int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particles");

	srand(time(NULL));

	//FireWorkSystem firework(10);
	vector<FireWork> firework;
	for (int i = 0; i < MAXNUM; i++)
		firework.push_back(FireWork(TURN));

	// create a clock to track the elapsed time
	sf::Clock clock;

	// run the main loop
	while (window.isOpen())
	{
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}


		// update it
		sf::Time elapsed = clock.restart();
		for (int i = 0; i<MAXNUM; i++)
			firework[i].fireworkShow(elapsed);

		// draw it
		window.clear();
		for (int i = 0; i<MAXNUM; i++)
			window.draw(firework[i]);
		window.display();
	}

	return 0;
}







