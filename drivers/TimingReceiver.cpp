#define ETHERBONE_THROWS 1

#include <sstream>
#include <algorithm>

#include "RegisteredObject.h"
#include "Driver.h"
#include "TimingReceiver.h"

namespace saftlib {

TimingReceiver::TimingReceiver(OpenDevice& od)
 : dev(od.device),
   name(od.name),
   etherbonePath(od.etherbonePath),
   sas_count(0)
{
}

void TimingReceiver::Remove()
{
  Directory::get()->RemoveDevice(name);
}

Glib::ustring TimingReceiver::getEtherbonePath() const
{
  return etherbonePath;
}

Glib::ustring TimingReceiver::getName() const
{
  return name;
}

void TimingReceiver::do_remove(Glib::ustring name)
{
  softwareActionSinks.erase(name);
  SoftwareActionSinks(getSoftwareActionSinks());
}


static inline bool not_isalnum_(char c)
{
  return !(isalnum(c) || c == '_');
}

Glib::ustring TimingReceiver::NewSoftwareActionSink(const Glib::ustring& name_)
{
  Glib::ustring name(name_);
  if (name == "") {
    std::ostringstream str;
    str.imbue(std::locale("C"));
    str << "_" << ++sas_count;
    name = str.str();
  } else {
    if (softwareActionSinks.find(name) != softwareActionSinks.end())
      throw Gio::DBus::Error(Gio::DBus::Error::INVALID_ARGS, "Name already in use");
    if (name[0] == '_')
      throw Gio::DBus::Error(Gio::DBus::Error::INVALID_ARGS, "Invalid name; leading _ is reserved");
    if (find_if(name.begin(), name.end(), not_isalnum_) != name.end())
      throw Gio::DBus::Error(Gio::DBus::Error::INVALID_ARGS, "Invalid name; [a-zA-Z0-9_] only");
  }
  
  // nest the object under our own name
  Glib::ustring path = getObjectPath() + "/SoftwareActionSinks/" + name;
  
  sigc::slot<void> destroy = sigc::bind(sigc::mem_fun(this, &TimingReceiver::do_remove), name);
  
  SoftwareActionSink::ConstructorType args = { this, 0, destroy };
  Glib::RefPtr<SoftwareActionSink> softwareActionSink = SoftwareActionSink::create(path, args);
  softwareActionSink->initOwner(getConnection(), getSender());
  
  softwareActionSinks[name] = softwareActionSink;
  SoftwareActionSinks(getSoftwareActionSinks());
  
  return path;
}

void TimingReceiver::InjectEvent(guint64 event, guint64 param, guint64 time)
{
  // !!!
}

guint64 TimingReceiver::getCurrentTime() const
{
  return 1; // !!!
}

std::map< Glib::ustring, Glib::ustring > TimingReceiver::getSoftwareActionSinks() const
{
  typedef std::map< Glib::ustring, Glib::RefPtr<SoftwareActionSink> >::const_iterator iterator;
  std::map< Glib::ustring, Glib::ustring > out;
  for (iterator i = softwareActionSinks.begin(); i != softwareActionSinks.end(); ++i)
    out[i->first] = i->second->getObjectPath();
  return out;
}

std::map< Glib::ustring, Glib::ustring > TimingReceiver::getOutputs() const
{
  std::map< Glib::ustring, Glib::ustring > out;
  return out; // !!! not for cryring
}

std::map< Glib::ustring, Glib::ustring > TimingReceiver::getInputs() const
{
  std::map< Glib::ustring, Glib::ustring > out;
  return out; // !!! not for cryring
}

std::map< Glib::ustring, Glib::ustring > TimingReceiver::getInoutputs() const
{
  std::map< Glib::ustring, Glib::ustring > out;
  return out; // !!! not for cryring
}

std::vector< Glib::ustring > TimingReceiver::getGuards() const
{
  std::vector< Glib::ustring > out;
  return out; // !!! not for cryring
}

std::map< Glib::ustring, std::vector< Glib::ustring > > TimingReceiver::getInterfaces() const
{
  std::map< Glib::ustring, std::vector< Glib::ustring > > out;
  return out; // !!!
}

guint32 TimingReceiver::getFree() const
{
  return 0; // !!!
}

void TimingReceiver::compile()
{
  // !!!
}

void TimingReceiver::probe(OpenDevice& od)
{
  // !!! check board ID
  od.ref = RegisteredObject<TimingReceiver>::create(od.objectPath, od);
}

static Driver<TimingReceiver> timingReceiver;

}
