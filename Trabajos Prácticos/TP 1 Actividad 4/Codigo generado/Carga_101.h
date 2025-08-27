
#ifndef CARGA_101_H
#define CARGA_101_H

#include "TP1Act4/Carga_Puntual_101.h"
#include "TP1Act4/Carga_Distribuida_101.h"
#include "TP1Act4/Carga_Puntual_101.h"

#include vector

class Viga_101;


namespace TP1Act4 {


/**
  * class Carga_101
  * 
  */

/******************************* Abstract Class ****************************
Carga_101 does not have any pure virtual methods, but its author
  defined it as an abstract class, so you should not use it directly.
  Inherit from it instead and create only objects from the derived classes
*****************************************************************************/

class Carga_101 : public Carga_Puntual_101, public Carga_Distribuida_101, public Carga_Puntual_101
{
public:
  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  Carga_101();

  /**
   * Empty Destructor
   */
  virtual ~Carga_101();

  // Static Public attributes
  //  

  // Public attributes
  //  

  double Magnitud_Equivalente;
  // En [rad]
  double Angulo_Equivalente;

  Viga_101 * m_viga;

  // Public attribute accessor methods
  //  


  // Public attribute accessor methods
  //  


  /**
   * Set the value of Magnitud_Equivalente
   * @param value the new value of Magnitud_Equivalente
   */
  void setMagnitud_Equivalente(double value)
  {
    Magnitud_Equivalente = value;
  }

  /**
   * Get the value of Magnitud_Equivalente
   * @return the value of Magnitud_Equivalente
   */
  double getMagnitud_Equivalente()
  {
    return Magnitud_Equivalente;
  }

  /**
   * Set the value of Angulo_Equivalente
   * En [rad]
   * @param value the new value of Angulo_Equivalente
   */
  void setAngulo_Equivalente(double value)
  {
    Angulo_Equivalente = value;
  }

  /**
   * Get the value of Angulo_Equivalente
   * En [rad]
   * @return the value of Angulo_Equivalente
   */
  double getAngulo_Equivalente()
  {
    return Angulo_Equivalente;
  }

  /**
   * Get the value of m_viga
   * @return the value of m_viga
   */
  Viga_101 getViga();

protected:
  // Static Protected attributes
  //  

  // Protected attributes
  //  


  // Protected attribute accessor methods
  //  


  // Protected attribute accessor methods
  //

private:
  // Static Private attributes
  //  

  // Private attributes
  //  


  // Private attribute accessor methods
  //  


  // Private attribute accessor methods
  //  


  void initAttributes();

};
} // end of package namespace

#endif // CARGA_101_H
