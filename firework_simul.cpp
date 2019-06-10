#include <SFML/Graphics.hpp>
#include <iostream>

using namespace std;

#define WIDTH	1000		
#define HEIGHT	600

#define TURN	360

#define MAXNUM		360

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

class Flame {

public:

	void setFlame(float m_angle,int m_lifetime) {
		
		float angle =  m_angle * 3.14f / 180.f;
		float speed = (rand() % 30) + 30.f;

		velocity = sf::Vector2f(cos(angle)*speed, sin(angle)*speed);

		lifetime = sf::seconds(m_lifetime);

		//color = sf::Color::Color(sf::Color::Red);
	}


	sf::Color color;

	sf::Vector2f velocity;
	sf::Time lifetime;


};


class FireWork : public sf::Drawable, public sf::Transformable {


public:

	FireWork(unsigned int count):
	flames(count),
	flame_vertices(sf::Points,count),
	start_vertice(sf::Points,1)
	{
		startFlame.setFlame(-90.f,7);
		start_vertice[0].position = sf::Vector2f(rand()%WIDTH,HEIGHT - 10);
		start_vertice[0].color = sf::Color::Red;
		startFlameAlive = true;

		m_lifetime = sf::seconds(5.f);
	}

	void flyingTotheSky(sf::Time elapsed) {

		startFlame.lifetime -= elapsed;
		if (startFlame.lifetime <= sf::Time::Zero) {
			startFlameAlive = false;

			for (int i = 0; i < flames.size(); i++) {
				flame_vertices[i].position = start_vertice[0].position;
				flame_vertices[i].color = sf::Color::Red;
				flames[i].setFlame(-(TURN/2) + (float)i,3);
			}

			return;
		}


		start_vertice[0].position += (elapsed.asSeconds() * startFlame.velocity);
	
	}

	void flameExplosion(sf::Time elapsed) {

		for (size_t i = 0; i < flames.size(); i++) {

			flames[i].lifetime -= elapsed;
			if (flames[i].lifetime <= sf::Time::Zero) {
				flames[i].lifetime = sf::seconds(0);
			}else
				flame_vertices[i].position += (elapsed.asSeconds() * flames[i].velocity);
		
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

		if(startFlameAlive == true)
			target.draw(start_vertice, states);
		else
			target.draw(flame_vertices , states);
	}

public:

	Flame startFlame;
	sf::VertexArray start_vertice;
	bool startFlameAlive;

	std::vector<Flame> flames;

	sf::VertexArray flame_vertices;

	sf::Time m_lifetime;

};


class FireWorkSystem{


public:

	FireWorkSystem(unsigned int count) :
	fireworks(count)
	{
		for (int i = 0; i < count; i++)
			fireworks[i] = FireWork(TURN);

	}


	void fireworkShow(sf::Time elapsed) {

		for (int i = 0; i < fireworks.size(); i++)
			fireworks[i].fireworkShow(elapsed);

	}


public:

	vector<FireWork> fireworks;



};


int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Particles");

	FireWorkSystem firework(10);

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
		firework.fireworkShow(elapsed);

		// draw it
		window.clear();
		for(int i=0;i<10;i++)
			window.draw(firework.fireworks[i]);
		window.display();
	}

	return 0;
}