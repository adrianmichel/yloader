/*
Copyright (C) 2017  YLoader.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "tree.h"

// T is the node type (Node or DocumentNode )
template <class T>
class Event {
 public:
  enum Type { add, remove, change };

 private:
  const Type _type;
  const T* _node;
  const UniqueId* _id;
  const UniqueId* _parentId;
  const bool _ensureVisible;

 public:
  Event(Type type, const T* node, const UniqueId* id, const UniqueId* parentId,
        bool ensureVisible)
      : _type(type),
        _node(node),
        _parentId(parentId),
        _ensureVisible(ensureVisible),
        _id(id) {}
  Type type() const { return _type; }
  const T* node() const { return _node; }
  const UniqueId* parentId() const { return _parentId; }
  bool ensureVisible() const { return _ensureVisible; }
  const UniqueId* id() const { return _id; }
};

template <class T>
class View {
 public:
  virtual ~View() {}

  virtual void update(const Event<T>& event) = 0;
  virtual bool allowRemove(const UniqueIdPtrVector& ids) const = 0;
  virtual bool allowChange(const UniqueIdPtrVector& ids) const = 0;
};

class Command {};

template <class T>
class Model {
 private:
  typedef std::set<View<T>*> ViewSet;
  ViewSet _views;

 public:
  Model() {}
  virtual ~Model() {
    // all views must have been removed from the model upon destruction
    assert(_views.size() == 0);
  }

  void addView(View<T>* view) {
    assert(view != 0);
    _views.insert(view);
  }

  void removeView(View<T>* view) {
    assert(view != 0);
    _views.erase(view);
  }

  virtual void command(Command cmd) = 0;

  void updateViews(const Event<T>& event) {
    for (ViewSet::iterator i = _views.begin(); i != _views.end(); i++)
      (*i)->update(event);
  }

  bool allowRemove(const UniqueIdPtrVector& ids) const {
    for (ViewSet::const_iterator i = _views.begin(); i != _views.end(); i++) {
      if (!(*i)->allowRemove(ids)) return false;
    }
    return true;
  }

  bool allowChange(const UniqueIdPtrVector& ids) const {
    for (ViewSet::const_iterator i = _views.begin(); i != _views.end(); i++) {
      if (!(*i)->allowChnage(ids)) return false;
    }
    return true;
  }
};

class Node : public Info {
 public:
  enum NodeType {
    MIN_NODE_TYPES,
    DATASOURCE,
    SYMBOLSSOURCE,
    RUNNABLE,
    SLIPPAGE,
    COMMISSION,
    SIGNALHANDLER,
    MAX_NODE_TYPES
  };

  enum NodeSubtype {
    MIN_NODE_SUBTYPES,
    CONFIG,
    PLUGIN,
    ROOT,
    MAX_NODE_SUBTYPES
  };

 public:
  Node(const Info& info) : Info(info) {}

  virtual ~Node() {}
  virtual NodeType type() const = 0;
  virtual NodeSubtype subtype() const = 0;

  static const std::wstring typeToString(NodeType type) {
    switch (type) {
      case DATASOURCE:
        return _T( "Data Source" );
      case SYMBOLSSOURCE:
        return _T( "Symbols Source" );
      case RUNNABLE:
        return _T( "Runnable" );
      case SLIPPAGE:
        return _T( "Slippage" );
      case COMMISSION:
        return _T( "Commission" );
      case SIGNALHANDLER:
        return _T( "Signal" );
      default:
        ASSERT(false);
        return _T( "" );
    }
  }
};

template <Node::NodeType t>
class NodeBase : public Node {
 public:
  NodeBase(const Info& info) : Node(info) {}
  NodeType type() const { return t; }
};

// config info
template <Node::NodeType t>
class ConfigNode : public NodeBase<t> {
 public:
  ConfigNode(const Info& info) : NodeBase<t>(info) {}

  virtual NodeSubtype subtype() const { return CONFIG; }
};

class Pathable {
 public:
  virtual ~Pathable() {}

  virtual const std::wstring& path() const = 0;
};

template <Node::NodeType t>
class PluginNode : public NodeBase<t>, public Pathable {
 private:
  const std::wstring _path;

 public:
  PluginNode(const Info& info, const std::wstring& path)
      : NodeBase<t>(info), _path(path) {}
  virtual NodeSubtype subtype() const { return PLUGIN; }
  const std::wstring& path() const { return _path; }
};

template <Node::NodeType t>
class PluginsRootNode : public NodeBase<t> {
 public:
  PluginsRootNode()
      : NodeBase<t>(Info(UniqueId(), makeName(), makeDescription())) {}

 private:
  static const std::wstring makeName() {
    switch (t) {
      case DATASOURCE:
        return _T( "Data Source Plugins" );
      case SYMBOLSSOURCE:
        return _T( "Symbols Source Plugins" );
      case RUNNABLE:
        return _T( "Runnable Plugins" );
      case SLIPPAGE:
        return _T( "Slippage Plugins" );
      case COMMISSION:
        return _T( "Commission Plugins" );
      case SIGNALHANDLER:
        return _T( "Signal Handler Plugins" );
      default:
        ASSERT(false);
        return _T( "" );
    }
  }
  static const std::wstring makeDescription() {
    switch (t) {
      case DATASOURCE:
        return _T( "Data Source Plugins" );
      case SYMBOLSSOURCE:
        return _T( "Symbols Source Plugins" );
      case RUNNABLE:
        return _T( "Runnable Plugins" );
      case SLIPPAGE:
        return _T( "Slippage Plugins" );
      case COMMISSION:
        return _T( "Commission Plugins" );
      case SIGNALHANDLER:
        return _T( "Signal Handler Plugins" );
      default:
        ASSERT(false);
        return _T( "" );
    }
  }

 public:
  virtual NodeSubtype subtype() const { return ROOT; }
};

typedef ManagedPtr<Node> NodePtr;

inline bool operator==(const NodePtr& node, const UniqueId& id) {
  return node->id() == id;
}

typedef ConfigNode<Node::NodeType::DATASOURCE> DataSourceConfigNode;
typedef ConfigNode<Node::NodeType::SYMBOLSSOURCE> SymbolsSourceConfigNode;
typedef ConfigNode<Node::NodeType::RUNNABLE> RunnableConfigNode;
typedef ConfigNode<Node::NodeType::COMMISSION> CommissionConfigNode;
typedef ConfigNode<Node::NodeType::SLIPPAGE> SlippageConfigNode;
typedef ConfigNode<Node::NodeType::SIGNALHANDLER> SignalHandlerConfigNode;

typedef PluginNode<Node::NodeType::DATASOURCE> DataSourcePluginNode;
typedef PluginNode<Node::NodeType::SYMBOLSSOURCE> SymbolsSourcePluginNode;
typedef PluginNode<Node::NodeType::RUNNABLE> RunnablePluginNode;
typedef PluginNode<Node::NodeType::COMMISSION> CommissionPluginNode;
typedef PluginNode<Node::NodeType::SLIPPAGE> SlippagePluginNode;
typedef PluginNode<Node::NodeType::SIGNALHANDLER> SignalHandlerPluginNode;

typedef PluginsRootNode<Node::NodeType::DATASOURCE> DataSourcePluginRootNode;
typedef PluginsRootNode<Node::NodeType::SYMBOLSSOURCE>
    SymbolsSourcePluginRootNode;
typedef PluginsRootNode<Node::NodeType::RUNNABLE> RunnablePluginRootNode;
typedef PluginsRootNode<Node::NodeType::COMMISSION> CommissionPluginRootNode;
typedef PluginsRootNode<Node::NodeType::SLIPPAGE> SlippagePluginRootNode;
typedef PluginsRootNode<Node::NodeType::SIGNALHANDLER>
    SignalHandlerPluginRootNode;

class Traverser {
 public:
  virtual ~Traverser() {}
  virtual void node(const Node* node) = 0;
};

class WrongPluginTypeException {
 private:
  const std::wstring _message;

 public:
  WrongPluginTypeException(const std::wstring& message) : _message(message) {}

  const std::wstring& message() const { return _message; }
};

class IdNotFoundException {
 private:
  const UniqueId _id;
  std::wstring _message;

 public:
  IdNotFoundException(const UniqueId& id) : _id(id) {
    std::wostringstream o;
    o << _T( "Id " ) << id << _T( " not found" );
    _message = o.str();
  }

  const std::wstring& message() const { return _message; }

  const UniqueId& id() const { return _id; }
};

class PluginTree : public tree<NodePtr>,
                   public Model<Node>,
                   public PluginExplorer {
 private:
  const iterator _dataSourcePluginRoot;
  const iterator _symbolsSourcePluginRoot;
  const iterator _runnablePluginRoot;
  const iterator _slippagePluginRoot;
  const iterator _commissionPluginRoot;
  const iterator _signalHandlerPluginRoot;

 public:
  PluginTree()
      : _dataSourcePluginRoot(
            insert(begin(), NodePtr(new DataSourcePluginRootNode()))),
        _symbolsSourcePluginRoot(
            insert(begin(), NodePtr(new SymbolsSourcePluginRootNode()))),
        _runnablePluginRoot(
            insert(begin(), NodePtr(new RunnablePluginRootNode()))),
        _commissionPluginRoot(
            insert(begin(), NodePtr(new CommissionPluginRootNode()))),
        _slippagePluginRoot(
            insert(begin(), NodePtr(new SlippagePluginRootNode()))),
        _signalHandlerPluginRoot(
            insert(begin(), NodePtr(new SignalHandlerPluginRootNode())))

  {}

  template <class T, class U, class V>
  void f(const std::wstring& filePath, iterator root) {
    try {
      PluginInstance<T> pi(filePath);
      addPlugin<Plugin<T>, U, V>(*pi, filePath, root);
    } catch (const PluginInstanceException&) {
      //			LOG( log_debug, _T( "PluginInstanceException: ")
      //<< filePath );
    }
  }

  virtual void process(const std::wstring& filePath,
                       PluginLoadingStatusHandler* loadingStatusHandler,
                       std::vector<InfoPtr>& duplicates) {
    try {
      if (loadingStatusHandler != 0) loadingStatusHandler->event(filePath);
      try {
        //				LOG( log_debug, _T( "trying data source
        //plugin: ") << filePath );
        f<DataSource, DataSourcePluginNode, DataSourceConfigNode>(
            filePath, _dataSourcePluginRoot);
      } catch (const PluginTreeException& e) {
        duplicates.insert(duplicates.end(), e.info().begin(), e.info().end());
      }

      try {
        f<SymbolsSource, SymbolsSourcePluginNode, SymbolsSourceConfigNode>(
            filePath, _symbolsSourcePluginRoot);
      } catch (const PluginTreeException& e) {
        duplicates.insert(duplicates.end(), e.info().begin(), e.info().end());
      }
      try {
        //				LOG( log_debug, _T( "trying runnable plugin:
        //") << filePath );
        f<Runnable, RunnablePluginNode, RunnableConfigNode>(
            filePath, _runnablePluginRoot);
      } catch (const PluginTreeException& e) {
        duplicates.insert(duplicates.end(), e.info().begin(), e.info().end());
      }
      try {
        f<Commission, CommissionPluginNode, CommissionConfigNode>(
            filePath, _commissionPluginRoot);
      } catch (const PluginTreeException& e) {
        duplicates.insert(duplicates.end(), e.info().begin(), e.info().end());
      }
      try {
        f<Slippage, SlippagePluginNode, SlippageConfigNode>(
            filePath, _slippagePluginRoot);
      } catch (const PluginTreeException& e) {
        duplicates.insert(duplicates.end(), e.info().begin(), e.info().end());
      }
      try {
        f<SignalHandler, SignalHandlerPluginNode, SignalHandlerConfigNode>(
            filePath, _signalHandlerPluginRoot);
      } catch (const PluginTreeException& e) {
        duplicates.insert(duplicates.end(), e.info().begin(), e.info().end());
      }
    } catch (const PluginMethodException&) {
      // the plugin doesn't implement at least one method
      //			LOG( log_debug, "PluginMethodException" );
    } catch (const PluginException&) {
      //			LOG( log_debug, "PluginException" );
    }
  }

  void init() {
    updateViews(Event<Node>(Event<Node>::Type::add,
                            (*_dataSourcePluginRoot).get(),
                            &(*_dataSourcePluginRoot)->id(), 0, false));
    updateViews(Event<Node>(Event<Node>::Type::add,
                            (*_symbolsSourcePluginRoot).get(),
                            &(*_symbolsSourcePluginRoot)->id(), 0, false));
    updateViews(Event<Node>(Event<Node>::Type::add,
                            (*_runnablePluginRoot).get(),
                            &(*_runnablePluginRoot)->id(), 0, false));
    updateViews(Event<Node>(Event<Node>::Type::add,
                            (*_commissionPluginRoot).get(),
                            &(*_commissionPluginRoot)->id(), 0, false));
    updateViews(Event<Node>(Event<Node>::Type::add,
                            (*_slippagePluginRoot).get(),
                            &(*_slippagePluginRoot)->id(), 0, false));
    updateViews(Event<Node>(Event<Node>::Type::add,
                            (*_signalHandlerPluginRoot).get(),
                            &(*_signalHandlerPluginRoot)->id(), 0, false));
  }

  virtual void command(Command cmd) {}

  // TODO: implement and check assumptions: level of various nodes, no duplicate
  // ids etc
  bool assertValid() { return true; }

  template <class T, Node::NodeType U>
  std::auto_ptr<PluginInstance<T> > getPlugin(const UniqueId& id) const
      throw(PluginMethodException, PluginInstanceException,
            WrongPluginTypeException, IdNotFoundException) {
    iterator i = std::find(begin(), end(), id);
    // todo: replace this assert with an exception - this is an error
    if (i != end()) {
      const Node* node = i->get();
      // if the node is of the wrong type, throw exception
      if (node->type() != U) {
        std::wostringstream o;

        o << _T( "Plugin " ) << id << _T( " is of type " )
          << Node::typeToString(node->type()) << _T( ", expected " )
          << Node::typeToString(U);
        throw WrongPluginTypeException(o.str());
      }
      assert(node->subtype() != Node::NodeSubtype::ROOT);
      // go down the tree until we find a parent that is of plugin type
      while (node->subtype() != Node::NodeSubtype::PLUGIN) {
        // call the base class parent
        i = tree<NodePtr>::parent(i);
        node = i->get();
      }

      const Pathable* pluginNode = dynamic_cast<const Pathable*>(node);

      return std::auto_ptr<PluginInstance<T> >(
          new PluginInstance<T>(pluginNode->path()));
    } else
      // couldn't find the id
      throw IdNotFoundException(id);
  }

  bool traverseChildren(const UniqueId& id, Traverser& traverser) const {
    for (iterator i = begin(); i != end(); i++) {
      if (*i == id) {
        for (sibling_iterator si = i; si != end(i); si++)
          traverser.node(si->get());

        return true;
      }
    }
    return false;
  }

 public:
  template <Node::NodeType T, Node::NodeSubtype U, class V>
  const V* findNode(const UniqueId& id) const {
    for (iterator i = begin(); i != end(); i++) {
      if (*i == id) {
        ASSERT((*i)->type() == T);
        ASSERT((*i)->subtype() == U);
        return dynamic_cast<const V*>(i->get());
      }
    }
    return 0;
  }

  const Node* findNode(const UniqueId& id) const {
    iterator i = std::find(begin(), end(), id);
    return i != end() ? i->get() : 0;
  }

  bool hasId(const UniqueId& id) const { return findNode(id) != 0; }

  template <Node::NodeType T>
  const PluginNode<T>* findPluginNode(const UniqueId& id) const {
    return findNode<T, Node::NodeSubtype::PLUGIN, PluginNode<T> >(id);
  }

  template <Node::NodeType T>
  const ConfigNode<T>* findConfigNode(const UniqueId& id) const {
    return findNode<T, Node::NodeSubtype::CONFIG, ConfigNode<T> >(id);
  }

  template <Node::NodeType T>
  const PluginsRootNode<T>* findPluginRootNode(const UniqueId& id) const {
    return findNode<T, Node::NodeSubtype::ROOT, PluginRootNode<T> >(id);
  }

  template <class V>
  void addConfig(const UniqueId& parent,
                 const Info& info) throw(PluginTreeException) {
    if (std::find(begin(), end(), info.id()) != end())
      // duplicate ids - throw exception
      throw PluginTreeException(info);
    iterator i = std::find(begin(), end(), parent);
    // parent must be in there already - assert if not
    assert(i != end());
    iterator k = append_child(i, NodePtr(new V(info)));
    updateViews(Event<Node>(Event<Node>::Type::add, (*k).get(), &(*k)->id(),
                            &(*i)->id(), false));
  }

  template <class V>
  void removeConfig(const UniqueId& id) {
    updateViews(Event<Node>(Event<Node>::Type::remove, 0, &id, 0, false));
    iterator i = std::find(begin(), end(), id);
    assert(i != end());
    __super::erase(i);
  }

  template <class V>
  void replaceConfig(const Info& info) throw(PluginTreeException) {
    iterator i = std::find(begin(), end(), info.id());
    assert(i != end());
    __super::replace(i, NodePtr(new V(info)));
    updateViews(
        Event<Node>(Event<Node>::Type::change, 0, &info.id(), 0, false));
  }

  bool checkIds(const UniqueId& id) {
    static UniqueId unauthorizedIds[] = {
        UniqueId(),
        UniqueId(_T( "02284BC4-CE53-4dd5-8925-B6B874799360" ))  // Greg's plugin
    };

    for (int n = 0; n < sizeof(unauthorizedIds) / sizeof(UniqueId); n++) {
      if (id == unauthorizedIds[n]) return false;
    }
    return true;
  }

  template <class T, class U, class V>
  void addPlugin(const T& plugin, const std::wstring& path,
                 const iterator& root) throw(PluginTreeException) {
    //		LOG( log_debug, _T( "trying to add plugin: ") << path );

    PluginTreeException e;
    // check that the plug-in is not on the black  list
    // if it is, just ignore it
    if (!checkIds(plugin.id())) return;
    // the node should not be there when adding
    const Node* node = findNode(plugin.id());

    // if the node is found, add it to the exception, the id should not be
    // duplicated
    if (node != 0)
      e.add(*node);
    else {
      iterator i = append_child(root, NodePtr(new U(plugin, path)));
      updateViews(Event<Node>(Event<Node>::Type::add, (*i).get(), &(*i)->id(),
                              &(*root)->id(), false));

      for (InfoPtr info = plugin.first(); info.get() != 0;
           info = plugin.next()) {
        try {
          addConfig<V>((*i)->id(), *info);
        } catch (const PluginTreeException& ex) {
          e.add(ex.info()[0]);
        }
      }
    }
    if (e.info().size() > 0) throw e;

    //		LOG( log_debug, _T( "plugin added: ") << path );
  }

  // returns the id of the plugin whose the parameter id is a configuration
  // if no parent, return 0
  const UniqueId* parent(const UniqueId& id) const throw(IdNotFoundException) {
    for (iterator i = begin(); i != end(); i++) {
      if (*i == id) {
        iterator p = tree<NodePtr>::parent(i);
        if (p != end())
          return &(*p)->id();
        else
          return 0;
      }
    }
    // the id wasn't found - throw and exception
    throw IdNotFoundException(id);
  }

 private:
};