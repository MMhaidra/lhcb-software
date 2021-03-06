/*============================================================
**
**      Finite state machine implementation to control 
**      and manipulate process groups
**
**  AUTHORS:
**
**      M.Frank  CERN/LHCb
**
**==========================================================*/
#ifndef ONLINE_FINITESTATEMACHINE_TRANSITION_H
#define ONLINE_FINITESTATEMACHINE_TRANSITION_H

// Framework include files
#include "FiniteStateMachine/TypedObject.h"
#include "FiniteStateMachine/Rule.h"

// C/C++ include files
#include <map>
#include <vector>

/* 
 *  FiniteStateMachine namespace declaration
 */
namespace FiniteStateMachine {

  // Forward declarations
  class Machine;
  class State;
  class Rule;

  /**@class TransitionActions Transition.h FiniteStateMachine/Transition.h
   *
   *  Definition of the state callback structure attached to a single FSM machine.
   *  While executing a transition 2 callbacks attached to a STATE may be called:
   *   o The state leave action (before the transition starts).
   *   o The state enter action (after the transition is completed).
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class TransitionActions    {
  protected:
    /// Pointer to transition preaction callback structure
    Callback m_pre;
    /// Pointer to transition action callback structure
    Callback m_action;
    /// Pointer to transition failaction callback structure
    Callback m_fail;
    /// Pointer to transition completion action callback structure
    Callback m_complete;

  public:
    /// Default Constructor
    TransitionActions();

    /** Class Constructor
     *  @arg  pre  [Callback,read-only]  Reference to transition pre action callback structure
     *  @arg  act  [Callback,read-only]  Reference to transition action callback structure
     *  @arg  fail [Callback,read-only]  Reference to transition fail action callback structure
     */
    TransitionActions(const Callback& pre, const Callback& act, const Callback& fail);

    /// Class Destructor
    virtual ~TransitionActions();

    /** Update callback structures for stat enter actions. 
     *  If a callback is already present, replace the old structure
     *  with the current staructure and delete the callback object.
     *
     *  @arg  cb  [Callback,read-only]  Reference to transition action callback structure
     */
    TransitionActions& setAction(const Callback& cb);

    /** Update callback structures for stat enter actions. 
     *  If a callback is already present, replace the old structure
     *  with the current staructure and delete the callback object.
     *
     *  @arg  cb  [Callback,read-only]  Reference to transition preaction callback structure
     */
    TransitionActions& setPreAction(const Callback& cb);

    /** Update callback structures for the transition failure actions. 
     *  If a callback is already present, replace the old structure
     *  with the current staructure and delete the callback object.
     *
     *  @arg  cb  [Callback,read-only]  Reference to transition fail-action callback structure
     */
    TransitionActions& setFailAction(const Callback& cb);

    /** Update callback structures for the transition completion actions. 
     *  If a callback is already present, replace the old structure
     *  with the current staructure and delete the callback object.
     *
     *  @arg  cb  [Callback,read-only]  Reference to transition fail-action callback structure
     */
    TransitionActions& setCompletionAction(const Callback& cb);

    /// Return pointer to transition action callback structure
    const Callback& action()  const     { return m_action;  }
    /// Return pointer to transition pre action callback structure
    const Callback& pre()  const        { return m_pre;     }
    /// Return pointer to transition fail action callback structure
    const Callback& fail()   const      { return m_fail;    }
    /// Return pointer to transition fail action callback structure
    const Callback& completion() const  { return m_complete;}
  };

  /**@class Transition  Transition.h FiniteStateMachine/Transition.h
   *
   *    Definition of the FSM ransition structure.
   *
   *    A transition is defined as the connection between
   *    an iniitial state and a target state. On executing the transition
   *    it is checked wether tasks have to be created and
   *    the proper callbacks are called if installed.
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  class Transition : public TypedObject  {
  public:
    /// Rule container definition
    typedef std::vector<Rule*> Rules;
    /// Predicate container definition
    typedef std::set<Predicate*> Predicates;
    enum TransitionFlags {
      NO_CHECKS       = 0,
      CREATE          = 1<<0,
      CHECK           = 1<<1,
      KILL            = 1<<2,
      ON_TIMEOUT_KILL = 1<<3,
      ___BLA
    };

  protected:
    /// Pointer to initial state
    const State* m_from;
    /// Pointer to target state
    const State* m_to;
    /// Set of transition rules
    Rules        m_rules;
    /// Set of transition predicates
    Predicates   m_predicates;
    /// Transition flags
    int          m_flags;

  public:
    /** Class Constructor	    
     * @arg nam   [string,read-only]   Transition name
     * @arg typ   [string,read-only]   Name of FSM type
     * @arg from  [State*,read-only]   Reference to FSM state object
     * @arg to    [State*,read-only]   Reference to FSM state object
     * @arg flags [int,   read-only]   Flag to steer task handling (See enum TransitionFlags)
     * @return Reference to self.
     */
    Transition (const Type* typ, const std::string& nam, const State* from, const State* to, int flags=CHECK);
    /// Class Destructor
    virtual ~Transition ();

    /// Return set of predicates attached to the FSM transition
    const Predicates& predicates () const   { return m_predicates;             }
    /// Return set of rules attached to the FSM transition
    const Rules& rules () const             { return m_rules;                  }

    /// Return pointer to initial state of FSM transition structure
    const State* from()  const              { return m_from;                   }
    /// Return pointer to target state of FSM transition structure  
    const State* to()  const                { return m_to;                     }
    /// Return flag to create tasks before invoking the transition
    bool create () const                    { return (m_flags&CREATE)==CREATE; }
    /// Return flag to fail the transiiton in case there are limbo tasks
    bool checkLimbo() const                 { return (m_flags&CHECK)==CHECK;   }
    /// Return flag to check if slaves should be killed before invoking transition
    bool killActive() const                 { return (m_flags&KILL)==KILL;     }
    /// Return flag to check if slaves should be killed on timeout
    bool onTimeoutKill() const
    { return (m_flags&ON_TIMEOUT_KILL)==ON_TIMEOUT_KILL;                       }

    /// Add a new predicate to a transition defining the allowed states of slaves for satisfication
    const Predicate* addPredicate(const Predicate::States& allowed);
    /// Add a new rule to a transition
    Transition& adoptRule(Rule* rule);
    /// Add a new rule to a transition
    Transition& adoptRule(const Transition* tr, Rule::Direction dir=Rule::MASTER2SLAVE);
    /// Add a new rules to a transition
    void adoptRule(Rules rules);
  };   //  End class Transition

  /**@class ObjectsOfType  Transition.h FiniteStateMachine/Transition.h
   *
   *    Helper class to add rules and predicates to transition objects
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct ObjectsOfType : public TypedObject {
    typedef std::map<std::string,const Transition*>  Transitions;
    typedef Transition::Rules Rules;
    Rule::Direction  direction;
    /// Class constructor
    ObjectsOfType(const Type* t, Rule::Direction d) : TypedObject(t,""), direction(d) {}
    /// Standard destructor
    virtual ~ObjectsOfType() {}
    /// Helper function to add predicates
    Predicate::States inState(const std::string& s1,    const std::string& s2="",
                              const std::string& s3="", const std::string& s4="", 
                              const std::string& s5="", const std::string& s6="",
                              const std::string& s7="", const std::string& s8="") const;
    /// Helper function to add predicates
    Predicate::States inState(const State* s1,   const State* s2=0,
                              const State* s3=0, const State* s4=0, 
                              const State* s5=0, const State* s6=0,
                              const State* s7=0, const State* s8=0
                              ) const;
    /// Helper function to add ANY rules
    Rule* moveTo(             const std::string& target_state) const;
    /// Helper function to add ANY rules
    Rule* moveTo(             const State* target_state) const;
    /// Helper function to add rules for transitions
    Rule* execute(     const Transition* tranition)  const;
    /// Helper function to add rules for transitions
    Rule* execTransition(     const std::string& curr_state, const std::string& target_state) const;
    /// Helper function to add rules for transitions
    Rule* execTransition(     const State* curr_state, const State* target_state) const;
    /// Helper function to define a whole set of rules to a transition depending on explicit transitions
    Rules execTransition(const Transitions& transitions)  const;
    /// Helper function to add rules for transitions
    Rule* move(              const std::string& curr_state, const std::string& target_state) const
    {  return execTransition(curr_state, target_state);   }
    /// Helper function to add rules for transitions
    Rule* move(               const State* curr_state, const State* target_state) const
    {  return execTransition(curr_state, target_state);   }
  };

  struct AllChildrenOfType : public ObjectsOfType {
    AllChildrenOfType(const Type* t) : ObjectsOfType(t,Rule::MASTER2SLAVE) {}
    virtual ~AllChildrenOfType() {}
  };
  struct ParentOfType : public ObjectsOfType {
    ParentOfType(const Type* t) : ObjectsOfType(t,Rule::SLAVE2MASTER) {}
    virtual ~ParentOfType() {}
  };

}      //  End namespace 
#endif //  ONLINE_FINITESTATEMACHINE_TRANSITION_H

