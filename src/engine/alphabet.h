// Define interface for the alphabet class.
#ifndef __ZUOPAR_ENGINE_ALPHABET_H__
#define __ZUOPAR_ENGINE_ALPHABET_H__

namespace ZuoPar {

namespace Engine {

class Alphabet {
public:
  Alphabet() {}

  /**
   * Insert the name into the alphabet.
   *
   *  @param[in]  name        The name string.
   *  @return     int         return 1 if the name is not in
   *                          the alphabet, otherwise false.
   */
  virtual int insert(const char* name) {};

  /**
   * Get the name of index from the alphabet.
   *
   *  @param[in]  id          The encoded index.
   *  @return     const char* The name string.
   */
  virtual const char* decode(int id) const = 0;

  /**
   * Get the index of the name from the alphabet.
   *
   *  @param[in]  name        The name string.
   *  @return     int         The encoded index.
   */
  virtual int encode(const char* name) const = 0;
};

}   //  end for namespace Engine

}   //  end for namespace ZuoPar

#endif  //  end for __ZUOPAR_ALPHABET_H__
