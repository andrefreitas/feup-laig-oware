#ifndef LSFANIMATION_H_
#define LSFANIMATION_H_

class Scenario {
public:
	virtual void draw();

};

class SandScenario:public Scenario{
public:
	void draw();
};

class JungleScenario:public Scenario{
public:
	void draw();
};


#endif