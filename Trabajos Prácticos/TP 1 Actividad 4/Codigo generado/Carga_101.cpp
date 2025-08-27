#include "Carga_101.h"

// Constructors/Destructors
//  

Carga_101::Carga_101()
{
  initAttributes();
}

Carga_101::~Carga_101()
{
}

//  
// Methods
//  


// Accessor methods
//  


/**
 * Get the value of m_viga
 * @return the value of m_viga
 */
Viga_101 * Carga_101::getViga()
{
  return m_viga;
}

// Other methods
//  

void Carga_101::initAttributes()
{
  m_viga = new Viga_101();
}

