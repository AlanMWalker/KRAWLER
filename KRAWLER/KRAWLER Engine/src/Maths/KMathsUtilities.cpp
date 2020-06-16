#include "Maths/KMathsUtilities.h"

void Krawler::Maths::InitRand()
{
    srand(static_cast<unsigned>(time(0)));
}

float Krawler::Maths::RandFloat()
{
    return (static_cast<float> (rand()) / static_cast<float>(RAND_MAX));
}

float Krawler::Maths::RandFloat(float Max)
{
    if (Max != 0)
    {
        return static_cast<float>(rand()) / static_cast<float> (RAND_MAX) / Max;
    }
    return(0.f);
}

float Krawler::Maths::RandFloat(float Min, float Max)
{
    if (Max - Min != 0)
    {
        return Min + static_cast<float> (rand()) / static_cast<float> (RAND_MAX) / (Max - Min);
    }
    return 0.f;
}

signed int Krawler::Maths::RandInt(signed int val)
{
    return rand() % val;
}
    