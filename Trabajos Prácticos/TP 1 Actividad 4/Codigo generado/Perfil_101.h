
#ifndef PERFIL_101_H
#define PERFIL_101_H

#include string
#include vector



namespace TP1Act4 {


/**
  * class Perfil_101
  * 
  */

class Perfil_101
{
public:
  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  Perfil_101();

  /**
   * Empty Destructor
   */
  virtual ~Perfil_101();

  // Static Public attributes
  //  

  // Public attributes
  //  


  // Public attribute accessor methods
  //  


  // Public attribute accessor methods
  //

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

  // En [MPa]
  unsigned int Esfuerzo_de_Von_Misses;
  // En [m²]
  unsigned int Área;
  // En [m³]
  unsigned int Momento_Primer_Orden;
  // En [m⁴]
  unsigned int Momento_Segundo_Orden;
  string Id_Estandar_del_Perfil;

  // Private attribute accessor methods
  //  


  // Private attribute accessor methods
  //  


  /**
   * Set the value of Esfuerzo_de_Von_Misses
   * En [MPa]
   * @param value the new value of Esfuerzo_de_Von_Misses
   */
  void setEsfuerzo_de_Von_Misses(unsigned int value)
  {
    Esfuerzo_de_Von_Misses = value;
  }

  /**
   * Get the value of Esfuerzo_de_Von_Misses
   * En [MPa]
   * @return the value of Esfuerzo_de_Von_Misses
   */
  unsigned int getEsfuerzo_de_Von_Misses()
  {
    return Esfuerzo_de_Von_Misses;
  }

  /**
   * Set the value of Área
   * En [m²]
   * @param value the new value of Área
   */
  void setÁrea(unsigned int value)
  {
    Área = value;
  }

  /**
   * Get the value of Área
   * En [m²]
   * @return the value of Área
   */
  unsigned int getÁrea()
  {
    return Área;
  }

  /**
   * Set the value of Momento_Primer_Orden
   * En [m³]
   * @param value the new value of Momento_Primer_Orden
   */
  void setMomento_Primer_Orden(unsigned int value)
  {
    Momento_Primer_Orden = value;
  }

  /**
   * Get the value of Momento_Primer_Orden
   * En [m³]
   * @return the value of Momento_Primer_Orden
   */
  unsigned int getMomento_Primer_Orden()
  {
    return Momento_Primer_Orden;
  }

  /**
   * Set the value of Momento_Segundo_Orden
   * En [m⁴]
   * @param value the new value of Momento_Segundo_Orden
   */
  void setMomento_Segundo_Orden(unsigned int value)
  {
    Momento_Segundo_Orden = value;
  }

  /**
   * Get the value of Momento_Segundo_Orden
   * En [m⁴]
   * @return the value of Momento_Segundo_Orden
   */
  unsigned int getMomento_Segundo_Orden()
  {
    return Momento_Segundo_Orden;
  }

  /**
   * Set the value of Id_Estandar_del_Perfil
   * @param value the new value of Id_Estandar_del_Perfil
   */
  void setId_Estandar_del_Perfil(string value)
  {
    Id_Estandar_del_Perfil = value;
  }

  /**
   * Get the value of Id_Estandar_del_Perfil
   * @return the value of Id_Estandar_del_Perfil
   */
  string getId_Estandar_del_Perfil()
  {
    return Id_Estandar_del_Perfil;
  }

  void initAttributes();

};
} // end of package namespace

#endif // PERFIL_101_H
