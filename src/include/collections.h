#pragma once

class ArrayIndexNotFoundException : public SimLibException
{
  public:
    ArrayIndexNotFoundException( size_t index )
    : SimLibException( ARRAY_INDEX_NOT_FOUND_EXCEPTION, std::wstring( _T( "Index not found in array: " ) ) << index )
    {
    }

};

template< class Value > class ARRAY
{
private:
  std::vector< Value > _v;

public:
  size_t count() const { return _v.size(); }
  size_t size() const { return _v.size(); }
  void add( const Value& v ) { _v.push_back( v ); }
  Value& operator[]( size_t index )
  { 
    if( index < _v.size() )
      return _v[ index ]; 
    else
      throw ArrayIndexNotFoundException( index );
  }

  bool empty() const { return _v.empty(); }
  void clear() { _v.clear(); }
};

class DictionaryKeyNotFoundException : public SimLibException
{
  public:
    DictionaryKeyNotFoundException()
    : SimLibException( DICTIONARY_KEY_NOT_FOUND_EXCEPTION, _T( "Key not found in dictionary collection" ) )
    {
    }
};

#define Array( Value ) ARRAY< Value >

template< class Key, class Value > class DICTIONARY
{
private:
  typedef std::map< Key, Value > Map;

  mutable Array( Key ) _keys;
  Map _m;
  mutable bool _clean;

public:
  DICTIONARY()
  : _clean( true )
  {
  }

  bool add( const Key& key, const Value& value )
  {
    std::pair< Map::iterator, bool > i = _m.insert( Map::value_type( key, value ) );
    if( i.second )
      _clean = false;
    return i.second;
  }

  Value& operator[]( const Key& key )
  {
    if( has( key ) )
      return _m[ key ];
    else
      throw DictionaryKeyNotFoundException();
  }


  bool has( const Key& key )
  {
    return _m.find( key ) != _m.end();
  }

  Array( Key ) getKeys() const
  {
    if( !_clean )
    {
      _keys.clear();
      for( Map::const_iterator i = _m.begin(); i != _m.end(); i++ )
      {
        _keys.add( (*i).first );
      }
      _clean = true;
    }

    return _keys;

  }
  
};

#define Dictionary( Key, Value ) DICTIONARY< Key, Value >
