
#ifndef VIGA_101_H
#define VIGA_101_H

#include string
#include vector

#include "Carga_101.h"


namespace TP1Act4 {


/**
  * class Viga_101
  * 
  */

class Viga_101
{
public:
  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  Viga_101();

  /**
   * Empty Destructor
   */
  virtual ~Viga_101();

  // Static Public attributes
  //  

  // En [m]
  static unsigned long int largo;
  // Public attributes
  //  

  TP1Act4::Material_101 Material;
  // Opcional
  TP1Act4::Tipo de Perfil_101 Tipo_de_Perfil;

  std::vector<Carga_101*> m_cargaVector;

  // Public attribute accessor methods
  //  


  /**
   * Set the value of largo
   * En [m]
   * @param value the new value of largo
   */
  void setLargo(unsigned long int value)
  {
    Viga_101::largo = value;
  }

  /**
   * Get the value of largo
   * En [m]
   * @return the value of largo
   */
  unsigned long int getLargo()
  {
    return Viga_101::largo;
  }

  // Public attribute accessor methods
  //  


  /**
   * Set the value of Material
   * @param value the new value of Material
   */
  void setMaterial(TP1Act4::Material_101 value)
  {
    Material = value;
  }

  /**
   * Get the value of Material
   * @return the value of Material
   */
  TP1Act4::Material_101 getMaterial()
  {
    return Material;
  }

  /**
   * Set the value of Tipo_de_Perfil
   * Opcional
   * @param value the new value of Tipo_de_Perfil
   */
  void setTipo_de_Perfil(TP1Act4::Tipo de Perfil_101 value)
  {
    Tipo_de_Perfil = value;
  }

  /**
   * Get the value of Tipo_de_Perfil
   * Opcional
   * @return the value of Tipo_de_Perfil
   */
  TP1Act4::Tipo de Perfil_101 getTipo_de_Perfil()
  {
    return Tipo_de_Perfil;
  }

  /**
   * Get the list of Carga objects held by m_cargaVector
   * @return std::vector<Carga_101> list of Carga objects held by m_cargaVector
   */
  std::vector<Carga_101> getCargaList();


  /**
   * @return double
   * @param  Magnitud En [N]
   * @param  Datos_de_Perfil
   */
  virtual double Esfuerzo_Normal(TP1Act4::Carga_101 Magnitud, TP1Act4::Perfil_101 Datos_de_Perfil) = 0;


  /**
   * @return double
   * @param  Magnitud En [N^`]
   */
  double Esfuerzo_Cortante(TP1Act4::Carga_101 Magnitud)
  {
  }


  /**
   * @return double
   * @param  Esfuerzo_Normal En [MPa]
   * @param  Esfuerzo_Cortante En [MPa]
   */
  double Von_Misses(double Esfuerzo_Normal, double Esfuerzo_Cortante)
  {
  }


  /**
   * En [cm²]
   * @return double
   * @param  Datos_de_Perfil Se requieren los valores de momentos de Inercia.
   */
  double Área_mínima(TP1Act4::Perfil_101 Datos_de_Perfil)
  {
  }


  /**
   */
  void Cantidad_de_Cargas()
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

#endif // VIGA_101_H
