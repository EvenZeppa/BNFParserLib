










// #include "BNFInterpreter.hpp"
// #include "Debug.hpp"
// #include <cstring>
// #include <sstream>

// static std::string stripAngles(const std::string& s) {
//     if (s.size() >= 2 && s[0] == '<' && s[s.size()-1] == '>') {
//         return s.substr(1, s.size()-2);
//     }
//     return s;
// }


// int BNFInterpreter::parse(const std::string& ruleName, const std::string& input, Value& out)
// {
//     if (!grammar) return -1;

//     std::string rn = ruleName;
//     if (!rn.empty() && rn[0] != '<') {
//         rn = "<" + rn + ">";
//     }

//     DEBUG_MSG("parse(): rule=" << rn << " input_len=" << input.size());

//     Rule* r = grammar->getRule(rn);
//     if (!r || !r->rootExpr) return -1;

//     size_t pos = 0;
//     // Ensure out is an object to receive results
//     out = Value();
//     out.type = Value::OBJECT;

//     int consumed = interpretExpr(r->rootExpr, input, pos, out);
//     if (consumed < 0) {
//         DEBUG_MSG("parse(): interpretExpr failed for rule=" << rn);
//         return -1;
//     }

//     DEBUG_MSG("parse(): consumed=" << pos);
//     return (int)pos;  // retourne le nombre de caractères consommés
// }

// void BNFInterpreter::mergeIntoKey(Value& dest, const std::string& key, const Value& item) {
//     if (key.empty()) return;

//     // Si la clé n'existe pas, on la crée
//     if (dest.type != Value::OBJECT) {
//         dest.type = Value::OBJECT;
//         dest.object.clear();
//     }

//     Value& target = dest.object[key];  // référence vers l'entrée (créée si inexistante)

//     // Fusion selon le type du nouvel item
//     switch (item.type) {
//         case Value::STRING:
//             if (target.type == Value::LIST) {
//                 target.list.push_back(item);
//             } else if (target.type == Value::STRING) {
//                 // transformer en liste
//                 Value old = target;
//                 target.type = Value::LIST;
//                 target.list.clear();
//                 target.list.push_back(old);
//                 target.list.push_back(item);
//             } else if (target.type == Value::OBJECT) {
//                 // impossible, mais on peut ignorer ou log
//                 std::cerr << "[WARN] mergeIntoKey: cannot merge string into object\n";
//             } else { // UNDEFINED
//                 target = item;
//             }
//             break;

//         case Value::LIST:
//             if (target.type != Value::LIST) {
//                 Value old = target;
//                 target.type = Value::LIST;
//                 target.list.clear();
//                 if (old.type != Value::UNDEFINED) target.list.push_back(old);
//             }
//             // fusionner tous les éléments
//             for (const auto& v : item.list) {
//                 target.list.push_back(v);
//             }
//             break;

//         case Value::OBJECT:
//             if (target.type != Value::OBJECT) {
//                 Value old = target;
//                 target.type = Value::OBJECT;
//                 target.object.clear();
//                 if (old.type != Value::UNDEFINED) {
//                     std::cerr << "[WARN] mergeIntoKey: object replaced non-object value\n";
//                 }
//             }
//             for (const auto& [k, v] : item.object) {
//                 mergeIntoKey(target, k, v);  // récursion sûre
//             }
//             break;
//     }
// }



// int BNFInterpreter::interpretExpr(Expression* expr, const std::string& input, size_t& offset, Value& out)
// {
//     if (!expr) return -1;

//     DEBUG_MSG("interpretExpr: type=" << expr->type << " offset=" << offset);

//     switch(expr->type) {
//         case Expression::EXPR_SEQUENCE:
//             return interpretSequence(expr, input, offset, out);
//         case Expression::EXPR_ALTERNATIVE:
//             return interpretAlternative(expr, input, offset, out);
//         case Expression::EXPR_REPEAT:
//             return interpretRepeat(expr, input, offset, out);
//         case Expression::EXPR_OPTIONAL:
//             return interpretOptional(expr, input, offset, out);
//         case Expression::EXPR_SYMBOL:
//             return interpretSymbol(expr, input, offset, out);
//         case Expression::EXPR_TERMINAL:
//             return interpretTerminal(expr, input, offset, out);
//         default:
//             return -1;
//     }
// }

// int BNFInterpreter::interpretSequence(Expression* seq, const std::string& input, size_t& offset, Value& out)
// {
//     DEBUG_MSG("interpretSequence: children=" << seq->children.size() << " offset=" << offset);

//     size_t start = offset;
//     Value local;
//     local.type = Value::OBJECT;

//     for (size_t i = 0; i < seq->children.size(); ++i) {
//         Value child;
//         child.type = Value::OBJECT;
//         int n = interpretExpr(seq->children[i], input, offset, child);
//         if (n < 0) return -1;
//         offset += n;

//         // merge each child result
//         for (std::map<std::string, Value>::iterator it = child.object.begin();
//              it != child.object.end(); ++it)
//         {
//             mergeIntoKey(local, it->first, it->second);
//         }
//     }

//     // final merge
//     for (std::map<std::string, Value>::iterator it = local.object.begin();
//          it != local.object.end(); ++it)
//     {
//         mergeIntoKey(out, it->first, it->second);
//     }

//     return offset - start;
// }


// int BNFInterpreter::interpretAlternative(Expression* alt, const std::string& input, size_t& offset, Value& out)
// {
//     DEBUG_MSG("interpretAlternative: options=" << alt->children.size() << " offset=" << offset);

//     size_t start = offset;

//     for (size_t i = 0; i < alt->children.size(); ++i) {
//         size_t pos = offset;
//         Value local; local.type = Value::OBJECT;
//         int n = interpretExpr(alt->children[i], input, pos, local);
//         if (n >= 0) {
//             offset = pos;
//             for (std::map<std::string, Value>::const_iterator it = local.object.begin(); it != local.object.end(); ++it)
//                 mergeIntoKey(out, it->first, it->second);
//             return (int)(offset - start);
//         }
//     }

//     return -1;
// }

// int BNFInterpreter::interpretRepeat(Expression* rep, const std::string& input, size_t& offset, Value& out)
// {
//     DEBUG_MSG("interpretRepeat: starting offset=" << offset);

//     size_t start = offset;
//     Value accumulated;
//     accumulated.type = Value::OBJECT;

//     while (true) {
//         size_t attempt = offset;
//         Value temp;
//         temp.type = Value::OBJECT;

//         int n = interpretExpr(rep->children[0], input, attempt, temp);
//         if (n < 0) break;

//         offset = attempt + n;

//         // merge the iteration
//         for (std::map<std::string, Value>::iterator it = temp.object.begin();
//              it != temp.object.end(); ++it)
//         {
//             mergeIntoKey(accumulated, it->first, it->second);
//         }
//     }

//     // merge accumulated list/object
//     for (std::map<std::string, Value>::iterator it = accumulated.object.begin();
//          it != accumulated.object.end(); ++it)
//     {
//         mergeIntoKey(out, it->first, it->second);
//     }

//     return offset - start;
// }


// int BNFInterpreter::interpretOptional(Expression* opt, const std::string& input, size_t& offset, Value& out)
// {
//     DEBUG_MSG("interpretOptional: offset=" << offset);

//     size_t start = offset;
//     size_t pos = offset;

//     Value local; local.type = Value::OBJECT;
//     int n = interpretExpr(opt->children[0], input, pos, local);

//     if (n >= 0) {
//         offset = pos;
//         for (std::map<std::string, Value>::const_iterator it = local.object.begin(); it != local.object.end(); ++it)
//             mergeIntoKey(out, it->first, it->second);
//         return (int)(offset - start);
//     }

//     return 0;
// }

// int BNFInterpreter::interpretSymbol(Expression* sym, const std::string& input, size_t& offset, Value& out)
// {
//     DEBUG_MSG("interpretSymbol: symbol='" << sym->value << "' offset=" << offset);
//     // lookup rule name normalized (sym->value can be "<letter>" or "letter")
//     std::string lookup = sym->value;
//     if (lookup.size() == 0) return -1;
//     if (lookup[0] != '<') { lookup = "<" + lookup + ">"; }

//     Rule* r = grammar->getRule(lookup);
//     if (!r || !r->rootExpr) {
//         DEBUG_MSG("interpretSymbol: rule not found for '" << lookup << "'");
//         return -1;
//     }

//     size_t start = offset;
//     Value temp;
//     temp.type = Value::OBJECT;

//     int n = interpretExpr(r->rootExpr, input, start, temp);
//     if (n < 0) {
//         DEBUG_MSG("interpretSymbol: inner rule did not match for '" << lookup << "'");
//         return -1;
//     }
//     // interpretExpr must have updated start to new offset; consumed = start - offset
//     size_t consumed = start - offset;
//     offset = start;

//     // Aggregate results from temp.object into a nice value under stripped key
//     std::string key = stripAngles(lookup); // e.g. "<letter>" -> "letter"

//     // If temp contains direct string(s) keyed by terminal literals, gather them into a list.
//     // Strategy: build a Value result:
//     Value result;
//     // default: if we can build a list of strings, make it a LIST; otherwise if empty, store matched substring.
//     std::vector<Value> gather;

//     for (std::map<std::string, Value>::const_iterator it = temp.object.begin();
//          it != temp.object.end(); ++it)
//     {
//         const Value& v = it->second;
//         if (v.type == Value::STRING) {
//             gather.push_back(v);
//         } else if (v.type == Value::LIST) {
//             // append all list elements
//             for (size_t i = 0; i < v.list.size(); ++i) gather.push_back(v.list[i]);
//         } else if (v.type == Value::OBJECT) {
//             // nested object: merge into out under their own keys (don't bury under symbol)
//             out.ensureObject();
//             out.mergeObjectInto(v.object); // using Value helper; if you didn't add it, do a manual merge here
//         }
//     }

//     if (!gather.empty()) {
//         result.type = Value::LIST;
//         result.list = gather;
//     } else {
//         // No terminal children collected: store the matched substring as STRING
//         result.type = Value::STRING;
//         result.str = input.substr(offset - consumed, consumed);
//     }

//     // Merge result under stripped symbol key
//     mergeIntoKey(out, key, result);

//     DEBUG_MSG("interpretSymbol: symbol='" << key << "' consumed=" << consumed);
//     return (int)consumed;
// }

// int BNFInterpreter::interpretTerminal(Expression* term, const std::string& input, size_t& offset, Value& out)
// {
//     DEBUG_MSG("interpretTerminal: literal='" << term->value << "' offset=" << offset);
//     size_t len = term->value.size();
//     if (offset + len <= input.size() && input.compare(offset, len, term->value) == 0) {
//         // create a plain STRING result with the literal match
//         Value v;
//         v.type = Value::STRING;
//         v.str = term->value;
//         // *** IMPORTANT: do NOT insert with key equal to the literal here ***
//         // Caller (interpretSymbol) will collect terminal strings from temp.object.
//         // However, when terminal is matched directly at top-level, we still want to expose it:
//         mergeIntoKey(out, "__terminal__", v); // temporary internal key, interpretSymbol will ignore
//         offset += len;
//         DEBUG_MSG("interpretTerminal: matched '" << term->value << "' len=" << len);
//         return (int)len;
//     }
//     DEBUG_MSG("interpretTerminal: did not match '" << term->value << "'");
//     return -1;
// }


