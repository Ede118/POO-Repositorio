
#ifndef CATÁLOGO_101_H
#define CATÁLOGO_101_H

#include string
#include vector



namespace TP1Act4 {


/**
  * class Catálogo_101
  * 
  */

class Catálogo_101
{
public:
  // Constructors/Destructors
  //  


  /**
   * Empty Constructor
   */
  Catálogo_101();

  /**
   * Empty Destructor
   */
  virtual ~Catálogo_101();

  // Static Public attributes
  //  

  // Public attributes
  //  


  // Public attribute accessor methods
  //  


  // Public attribute accessor methods
  //  



  /**
   * @return TP1Act4::Perfil_101
   * @param  Von_Misses En [MPa]
   * @param  Área_mínima En [cm²]
   * @param  Material
   * @param  Tipo_de_Perfil Opcional
   */
  TP1Act4::Perfil_101 Buscar_Perfil(double Von_Misses, double Área_mínima, string Material, string Tipo_de_Perfil)
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

  unsigned int Precio;
  unsigned int Proveedor;
  string Nomenclatura_Perfil;

  // Private attribute accessor methods
  //  


  // Private attribute accessor methods
  //  


  /**
   * Set the value of Precio
   * @param value the new value of Precio
   */
  void setPrecio(unsigned int value)
  {
    Precio = value;
  }

  /**
   * Get the value of Precio
   * @return the value of Precio
   */
  unsigned int getPrecio()
  {
    return Precio;
  }

  /**
   * Set the value of Proveedor
   * @param value the new value of Proveedor
   */
  void setProveedor(unsigned int value)
  {
    Proveedor = value;
  }

  /**
   * Get the value of Proveedor
   * @return the value of Proveedor
   */
  unsigned int getProveedor()
  {
    return Proveedor;
  }

  /**
   * Set the value of Nomenclatura_Perfil
   * @param value the new value of Nomenclatura_Perfil
   */
  void setNomenclatura_Perfil(string value)
  {
    Nomenclatura_Perfil = value;
  }

  /**
   * Get the value of Nomenclatura_Perfil
   * @return the value of Nomenclatura_Perfil
   */
  string getNomenclatura_Perfil()
  {
    return Nomenclatura_Perfil;
  }

  void initAttributes();

};
} // end of package namespace

#endif // CATÁLOGO_101_H
