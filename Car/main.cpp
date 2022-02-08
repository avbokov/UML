#include<iostream>
#include<iomanip>
#include<thread>
#include<chrono>
#include<conio.h>
#include<Windows.h>
using namespace std;

#define MIN_TANK_VOLUME 20
#define MAX_TANK_VOLUME 150

class Tank
{
	const unsigned int VOLUME;	// Объём бака (характеристика объекта)
	double fuel_level;			// Уровень топлива в баке (состояние объекта)

public:

	unsigned int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0) return;
		if (fuel_level + fuel <= VOLUME) fuel_level += fuel;
		else fuel_level = VOLUME;
	}
	double give_fuel(double amount)
	{
		if (fuel_level < amount)return fuel_level = 0;
		fuel_level -= amount;
		/*if (fuel_level - amount > 0)fuel_level -= amount;    // Метод Славы
		else fuel_level = 0;*/
		return fuel_level;
	}
	Tank(double VOLUME):VOLUME(VOLUME >= MIN_TANK_VOLUME && VOLUME <= MAX_TANK_VOLUME? VOLUME : 80), fuel_level(0)
	{
		/*if (VOLUME >= MIN_TANK_VOLUME && VOLUME <= MAX_TANK_VOLUME)
			this->VOLUME = VOLUME;*/
		cout << "Tank is ready" << endl;
	}
	~Tank()
	{
		cout << "Tank is over" << endl;
	}

	void info()const
	{
		cout << "Volume:     " << VOLUME << " liters.\n";
		cout << "Fuel level: " << fuel_level << " liters.\n";
	}
};

#define MIN_ENGINE_CONSUMPTION 6
#define MAX_ENGINE_CONSUMPTION 25

class Engine
{
	double default_consumption;	// Расход топлива на 100 км
	double consumption;	// Расход топлива на 100 км
	double consumption_per_second;	// Расход топлива за 1 секунду
	bool is_started;	// Заведён или заглушен

public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	double get_consumption_per_hour()const
	{
		return consumption_per_second * 3600;
	}
	bool started()const
	{
		return is_started;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}

	void set_consumption(double consumption)
	{
		if (consumption < MIN_ENGINE_CONSUMPTION)consumption = MIN_ENGINE_CONSUMPTION;
		else if (consumption > MAX_ENGINE_CONSUMPTION)consumption = MAX_ENGINE_CONSUMPTION;
		this->consumption = consumption;
		this->consumption_per_second = this->consumption * .3e-4;
	}
	Engine(double consumption)
	{
		set_consumption(consumption);
		this->default_consumption = this->consumption;
		is_started = false;
		cout << "Engine is ready" << endl;
	}
	~Engine()
	{
		cout << "Engine is over" << endl;
	}

	void change_consumption(int speed)
	{
		set_consumption(default_consumption);
		if (speed > 0 && speed <= 60) consumption_per_second *= 6;
		else if (speed > 60 && speed <= 100) consumption_per_second *= 5;
		else if (speed > 100 && speed <= 140) consumption_per_second *= 6;
		else if (speed > 140 && speed <= 200) consumption_per_second *= 8;
		else if (speed > 200 && speed <= 250) consumption_per_second *= 10;
		
	}

	void info()const
	{
		cout << "Consumption per 100 km: " << consumption << " liters.\n";
		cout << "Consumption per second: " << consumption_per_second << " liters.\n";
		cout << "Consumption per hour: " << get_consumption_per_hour() << " liters.\n";
		cout << "Engine is " << (is_started ? "started" : "stopped") << endl;		
	}
};

//#define HOMEWORK

#ifdef HOMEWORK

const unsigned int MAX_SPEED_DEFAULT = 200;

class Car
{
	Engine engine;
	Tank tank;

	unsigned int current_speed;
	unsigned int max_speed;

public:

	Car(double consumption, const unsigned int VOLUME, unsigned int max_speed) : engine(consumption), tank(VOLUME), max_speed(max_speed >= 100 && max_speed <= 200 ? max_speed : MAX_SPEED_DEFAULT)
	{
		current_speed = 0;
		cout << "Car is ready" << endl;
	}
	~Car()
	{
		cout << "Car is over" << endl;
	}

	void info()const
	{
		tank.info();
		engine.info();
		cout << "Current speed: " << current_speed << " km/h" << endl;;
		cout << "Max speed:\t" << max_speed << " km/h" << endl;;
	}
};
#endif // HOMEWORK

//enum Key
//{
//	Enter = 13,
//	Escape = 27
//};
#define Enter 13
#define Escape 27

class Car
{
	Engine engine;
	Tank tank;
	bool driver_inside;
	int speed;
	const int MAX_SPEED;
	int acceleration;

	struct Control 
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeleing_thread;
	}control;

public:
	Car(double consumption, unsigned int volume, int max_speed, int acceleration=10) :
		engine(consumption),
		tank(volume),
		MAX_SPEED(max_speed>=80 && max_speed<=400? max_speed :180)
	{
		driver_inside = false;
		speed = 0;
		this->acceleration = acceleration;
		cout << "Your car is ready: " << this << endl;
	}

	~Car()
	{
		cout << "Your car is over: " << this << endl;
	}
	void fill(double fuel)
	{
		tank.fill(fuel);
	}
	void get_in()
	{
		driver_inside = true;
		control.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (control.panel_thread.joinable())control.panel_thread.join();
		system("CLS");
		cout << "You are out of your car, press Enter to get in" << endl;
	}
	void start_engine()
	{
		if (driver_inside && tank.give_fuel(engine.get_consumption_per_second()))
		{
			engine.start();
			control.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop_engine()
	{
		engine.stop();
		if (control.engine_idle_thread.joinable())control.engine_idle_thread.join();
	}

	void control_car()
	{
		cout << "Press Entre to get in" << endl;
		char key;
		do
		{
			key = _getch();
			switch (key)
			{
			case 'F':case 'f':
				double fuel;
				cout << "Введите объём топлива: "; cin >> fuel;
				fill(fuel);
				break;
			case 'I':case'i':
				if (engine.started())stop_engine();
				else start_engine();
				break;
			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break;
			case 'W':case'w':
				if (driver_inside && engine.started())
				{
					if(speed < MAX_SPEED)speed += acceleration;
					if (speed > MAX_SPEED)speed = MAX_SPEED;
					if (!control.free_wheeleing_thread.joinable())
						control.free_wheeleing_thread = std::thread(&Car::free_wheeling, this);
					std::this_thread::sleep_for(1s);
				}
				break;
			case 'S':case's':
				if (speed > 0)speed -= acceleration * 2;
				if (speed < 0)speed = 0;
				std::this_thread::sleep_for(1s);
				break;
			case Escape:
				speed = 0;
				stop_engine();
				get_out();
			}
			if (tank.get_fuel_level() == 0)stop_engine();
			if (speed == 0 && control.free_wheeleing_thread.joinable()) control.free_wheeleing_thread.join();
		} while (key != Escape);
	}

	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
		{
			std::this_thread::sleep_for(1s);
		}
		//stop_engine();
		engine.stop();
	}
	void free_wheeling()
	{
		while (speed > 0)
		{
			speed--;
			engine.change_consumption(speed);
			std::this_thread::sleep_for(1s);
		}
		engine.change_consumption(speed);
	}

	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			for (int i = 0; i < speed*0.4; i++)
			{
				if (i > 50)SetConsoleTextAttribute(hConsole, 0x0E);
				if (i > 80)SetConsoleTextAttribute(hConsole, 0x0C);
				cout << "|";
			}
			SetConsoleTextAttribute(hConsole, 0x07);
			cout << endl;
			cout << "Fuel level " << std::setprecision(4) << fixed << tank.get_fuel_level() << " liters.\t";
			if (tank.get_fuel_level() < 5)
			{
				SetConsoleTextAttribute(hConsole, 0x60);
				//SetConsoleTextAttribute(hConsole, 0x06);
				cout << "LOW FUEL";
				SetConsoleTextAttribute(hConsole, 0x07);

			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			cout << "Speed: " << speed << " km/h\n";
			cout << "Consumption: " << engine.get_consumption_per_second() << " liters/s\n";
			std::this_thread::sleep_for(500ms);
		}
	}

	void info()const
	{
		engine.info();
		tank.info();
		cout << "Max speed:\t" << MAX_SPEED << " km/h\n";
	}


};

//#define TANK_CHECK
//#define ENGINE_CHECK

void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHECK
	Tank tank(40);
	tank.info();
	int fuel;
	while (true)
	{
		cout << "Введите объём топлива: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	}
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(8);
	engine.info();
#endif // ENGINE_CHECK
		
#ifdef HOMEWORK
	Car car(100, 60, 210);
	car.info();
#endif // HOMEWORK

	Car bmw(11, 80, 250, 20);
	//bmw.info();
	bmw.control_car();

}