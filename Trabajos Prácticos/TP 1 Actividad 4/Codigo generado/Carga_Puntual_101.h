
#ifndef CARGA_PUNTUAL_101_H
#define CARGA_PUNTUAL_101_H

#include "TP1Act4/Carga_101.h"
#include "TP1Act4/Carga_101.h"

#include vector



namespace TP1Act4 {


/**
  * class Carga_Puntual_101
  * 
  */

class Carga_Puntual_101 : virtual public Carga_101, virtual public Carga_101
{
public:
  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  Carga_Puntual_101();

  /**
   * Empty Destructor
   */
  virtual ~Carga_Puntual_101();

  // Static Public attributes
  //  

  // Public attributes
  //  

  // En [m]
  int Posición_x;
  // En [m]
  int Posición_y;

  // Public attribute accessor methods
  //  


  // Public attribute accessor methods
  //  


  /**
   * Set the value of Posición_x
   * En [m]
   * @param value the new value of Posición_x
   */
  void setPosición_x(int value)
  {
    Posición_x = value;
  }

  /**
   * Get the value of Posición_x
   * En [m]
   * @return the value of Posición_x
   */
  int getPosición_x()
  {
    return Posición_x;
  }

  /**
   * Set the value of Posición_y
   * En [m]
   * @param value the new value of Posición_y
   */
  void setPosición_y(int value)
  {
    Posición_y = value;
  }

  /**
   * Get the value of Posición_y
   * En [m]
   * @return the value of Posición_y
   */
  int getPosición_y()
  {
    return Posición_y;
  }

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

#endif // CARGA_PUNTUAL_101_H
