
#ifndef CARGA_DISTRIBUIDA_101_H
#define CARGA_DISTRIBUIDA_101_H

#include "TP1Act4/Carga_101.h"

#include string


namespace TP1Act4 {


/**
  * class Carga_Distribuida_101
  * 
  */

class Carga_Distribuida_101 : virtual public Carga_101
{
public:
  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  Carga_Distribuida_101();

  /**
   * Empty Destructor
   */
  virtual ~Carga_Distribuida_101();

  // Static Public attributes
  //  

  // Public attributes
  //  

  // En [m]
  double x_inicial;
  // En [m]
  double y_inicial;
  // En [m]
  double x_final;
  // En [m]
  double y_final;
  // Función de distribución
  TP1Act4::funcion q_x_y_;

  // Public attribute accessor methods
  //  


  // Public attribute accessor methods
  //  


  /**
   * Set the value of x_inicial
   * En [m]
   * @param value the new value of x_inicial
   */
  void setX_inicial(double value)
  {
    x_inicial = value;
  }

  /**
   * Get the value of x_inicial
   * En [m]
   * @return the value of x_inicial
   */
  double getX_inicial()
  {
    return x_inicial;
  }

  /**
   * Set the value of y_inicial
   * En [m]
   * @param value the new value of y_inicial
   */
  void setY_inicial(double value)
  {
    y_inicial = value;
  }

  /**
   * Get the value of y_inicial
   * En [m]
   * @return the value of y_inicial
   */
  double getY_inicial()
  {
    return y_inicial;
  }

  /**
   * Set the value of x_final
   * En [m]
   * @param value the new value of x_final
   */
  void setX_final(double value)
  {
    x_final = value;
  }

  /**
   * Get the value of x_final
   * En [m]
   * @return the value of x_final
   */
  double getX_final()
  {
    return x_final;
  }

  /**
   * Set the value of y_final
   * En [m]
   * @param value the new value of y_final
   */
  void setY_final(double value)
  {
    y_final = value;
  }

  /**
   * Get the value of y_final
   * En [m]
   * @return the value of y_final
   */
  double getY_final()
  {
    return y_final;
  }

  /**
   * Set the value of q_x_y_
   * Función de distribución
   * @param value the new value of q_x_y_
   */
  void setQ_x_y_(TP1Act4::funcion value)
  {
    q_x_y_ = value;
  }

  /**
   * Get the value of q_x_y_
   * Función de distribución
   * @return the value of q_x_y_
   */
  TP1Act4::funcion getQ_x_y_()
  {
    return q_x_y_;
  }


  /**
   */
  void getDistribución()
  {
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

#endif // CARGA_DISTRIBUIDA_101_H
