/**
 * @file punctuators.cpp
 * Manages the table of punctuators.
 *
 * @author  Ben Gardner
 * @license GPL v2+
 */
#include "punctuators.h"
#include "prototypes.h"
#include <algorithm>


using namespace std;


/**
 *
 *   The file "punctuator_table.h" was generated by punc.py from this file.
 *   If you change one of the below symbol tables, re-run punc.py.
 *   $ python scripts/punc.py > src/punctuator_table.h
 *   now automatically with CMakeLists.txt
 *
 *   NOTE: the tables below do not need to be sorted.
 */

#include "symbols_table.h"
// must be positioned here, after all symbolsX[], because punc_table uses them
#include <punctuator_table.h>


const chunk_tag_t *find_punctuator(const char *str, int lang_flags)
{
   if (str == nullptr || str[0] == '\0')
   {
      return(nullptr);
   }

   const auto binary_find =
      [](const lookup_entry_t *first, const lookup_entry_t *last, const char &value)
      {
         const auto tmp = std::lower_bound(first, last, value,
                                           lookup_entry_t::comperator());

         return((value == tmp->ch) ? tmp : nullptr);
      };

   const chunk_tag_t *match  = nullptr;
   const auto        *parent = punc_table; //!< graph in table form, initially point at first entry
   auto              ch_idx  = int {};

   while (ch_idx < 6 && str[ch_idx] != '\0')  //!< symbols6: max punc len = 6
   {
      // search for next parent node in all current child nodes
      parent = binary_find(parent, next(parent, parent->left_in_group), str[ch_idx]);
      if (parent == nullptr)
      {
         break; // no nodes found with the searched char
      }

      if (  parent->tag != nullptr
         && (parent->tag->lang_flags & lang_flags) != 0   // punctuator lang and processing lang match
         && (  (parent->tag->lang_flags & FLAG_DIG) == 0  // punctuator is not a di/tri-graph
            || cpd.settings[UO_enable_digraphs].b))       // or di/tri-graph processing is enabled
      {
         match = parent->tag;
      }

      if (parent->next_idx == 0)
      {
         break;                               // no child nodes, leaf reached
      }
      parent = &punc_table[parent->next_idx]; // point at the first child node
      ch_idx++;
      continue;
   }
   return(match);
} // find_punctuator
