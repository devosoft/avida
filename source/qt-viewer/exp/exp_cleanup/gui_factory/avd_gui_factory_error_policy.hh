#ifndef AVD_GUI_FACTORY_ERROR_POLICY_HH
#define AVD_GUI_FACTORY_ERROR_POLICY_HH


template <typename IdentifierType, class AbstractProduct>
struct avdControllerFactoryErrorPolicy {
  static AbstractProduct* OnUnknownType(IdentifierType) {
    Error << "trying to instantiate unknown class type.";
    return 0;
} };

template <class AbstractProduct>
struct avdControllerFactoryErrorPolicy<QString, AbstractProduct> {
  static AbstractProduct* OnUnknownType(const QString &id) {
    Error
    << "\n"
    << "\n\tFor some reason, class \"" << id << "\""
    << "\n\tis not registered with the GUI creation code."
    << "\n\tPerhaps it was not compiled into this version of Avida?"
    << "\n";
    return 0;
} };

#endif

// arch-tag: header file for generic gui object factory error policy
