#include "Viga_101.h"

// Constructors/Destructors
//  

Viga_101::Viga_101()
{
  initAttributes();
}

Viga_101::~Viga_101()
{
}

//  
// Methods
//  


// Accessor methods
//  


/**
 * Get the list of Carga objects held by m_cargaVector
 * @return std::vector<Carga_101 *> list of Carga objects held by m_cargaVector
 */
std::vector<Carga_101 *> Viga_101::getCargaList() {
  return m_cargaVector;
}

// Other methods
//  

void Viga_101::initAttributes()
{
  Material = Acero;
}

