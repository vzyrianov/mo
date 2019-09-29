#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

enum symbol {
   string,
   arrow, 
   colon,
   square_bracket_open,
   square_bracket_close,
   newline,
   eof
};

struct token {
   symbol sym;
   std::string value;
};

std::ifstream input;

void print(symbol sym) {
   switch (sym) {
      case string:
         std::cout << "string";
         break;
      case arrow:
         std::cout << "arrow";
         break;
      case colon:
         std::cout << "colon";
         break;
      case square_bracket_open:
         std::cout << "square bracket opened";
         break;
      case square_bracket_close:
         std::cout << "square bracket closed";
         break;
      case newline:
         std::cout << "newline";
         break;
      case eof:
         std::cout << "eof";
         break;
   }
}

token next_token() {
   token s;
   char c;

   s.sym = eof;


   if (input >> c) {
      if (c == '[')
         s.sym = square_bracket_open;
      else if (c == ']')
         s.sym = square_bracket_close;
      else if (c == ':')
         s.sym = colon;
      else if (c == '-' && (char)input.peek() == '>') {
         s.sym = arrow;
         input >> c;
      }
      else if (c == '\n')
         s.sym = newline;
      else if (c == '\r')
         s.sym = newline;
      else {
         s.sym = string;

         s.value += c;
         c = (char) input.peek();
         while (!input.eof() && c != '[' && c != ']' && c != ' ' && c != '-' && c != ':' && c != '\r' && c != '\n') {
            s.value += c;
            input >> c;
            c = (char)input.peek();
         }
      }
   } 

   return s;
}

struct node {
   std::string filename;
   int thread_count;

   std::string getFunctionName() {
      return filename.substr(0, filename.find_first_of("."));
   }
};

token current_token;

std::map<std::string, node> definitions;
std::map<std::string, std::set<std::string>> connections;

bool accept() {
   if (!input.eof()) {
      current_token = next_token();
      return true;
   }

   return false;
}

bool expect(symbol s) {
   token next = next_token();

   if (next.sym == s) {
      current_token = next;
      return true;
   }
   else {
      std::cout << "Error: expected ";
      print(s);
      std::cout << std::endl;
   }

   return false;
}

std::string key;
int thread_count;

bool handle_square_brackets() {
   if (!expect(string))
      return false;

   //TODO PARSE IT.
   thread_count = 10;

   if (!expect(square_bracket_close))
      return false;
   return true;
}

bool handle_assignment () {
   if (!accept())
      return false;

   if (current_token.sym == square_bracket_open) {
      if (!handle_square_brackets())
         return false;

      if (!expect(string))
         return false;
   }
   else {
      thread_count = 1;
   }


   node n;
   n.filename = current_token.value;
   n.thread_count = thread_count;

   definitions[key] = n;

   return true;
}

bool handle_link() {
   if (!expect(string))
      return false;

   connections[key].insert(current_token.value);

   return true;
}

bool read_program() {
   while (true) {

      accept();

      if(current_token.sym == eof)
         return true;
      if(current_token.sym != string) {
         std::cout << "Expected a string";
         return false;
      }

      key = current_token.value;

      accept();

      if (current_token.sym == colon) {
         if (!handle_assignment())
            return false;
      } else if (current_token.sym == arrow) {
         if (!handle_link())
            return false;
      } else if (current_token.sym == eof) {
         return true;
      } else {
         print(current_token.sym);
         std::cout << "printed in read_program value; " << current_token.value << std::endl;
         std::cout << "Expected : or -> " << std::endl;
         return false;
      }
   }
}

// std::map<std::string, node> definitions;
// std::map<std::string, std::set<std::string>> connections;

void generate_asm() {
   std::cout << "extern malloc" << std::endl;
   std::cout << "extern pthread_mutex_init" << std::endl;
   std::cout << "extern pthread_create" << std::endl;
   std::cout << "extern pthread_join" << std::endl;

   std::cout << std::endl;

   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "extern " << it->second.getFunctionName() << std::endl;
   }

   std::cout << "section .data" << std::endl;

   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\tpthread_" << it->second.getFunctionName();
      std::cout << " resq 1" << std::endl;
   }

   std::cout << std::endl;

   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\tparam_" << it->second.getFunctionName();
      std::cout << " rest 4" << std::endl;
   }

   std::cout << std::endl;


   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\twrite_head_" << it->second.getFunctionName();
      std::cout << " resq 1" << std::endl;
   }


   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\twrite_mutex_" << it->second.getFunctionName();
      std::cout << " rest 4" << std::endl;
   }

   std::cout << "\tstop dd 0" << std::endl;

   std::cout << "section .text" << std::endl;
   std::cout << "\tglobal main" << std::endl;

   std::cout << "main:" << std::endl;
   std::cout << "\tpush r12" << std::endl;

   for(auto it = definitions.begin(); it != definitions.end(); ++it) {

      //Initialize read_head
      std::cout << "; initialize read_head" << std::endl;
      std::cout << "\tmov rdi, 16" << std::endl;
      std::cout << "\tcall malloc" << std::endl;
      std::cout << "\tmov r12, rax" << std::endl; //Initial node in r12
      std::cout << "\tmov r13, 0" << std::endl;
      std::cout << "\tmov [r12+8], r13" << std::endl; //Clear it

      //Move initial node to this node's write_head
      std::cout << "\tmov [write_head_" << it->second.getFunctionName() << "], r12"
         << std::endl;

      std::cout << "\tmov rdi, 8" << std::endl;
      std::cout << "\tcall malloc" << std::endl;
      std::cout << "\tmov [param_" << it->second.getFunctionName() << "], rax";
      std::cout << std::endl;
      std::cout << "\tmov [rax], r12;" << std::endl;

      //Initialize read_mutex
      std::cout << "; initialize read_mutex" << std::endl;
      std::cout << "\tmov rdi, 40" << std::endl;
      std::cout << "\tcall malloc" << std::endl;
      std::cout << "\tmov [param_" << it->second.getFunctionName() << "+8], rax";
      std::cout << std::endl;
      std::cout << "\tmov rdi, rax" << std::endl;
      std::cout << "\tmov rsi, 0" << std::endl;
      std::cout << "\tcall pthread_mutex_init" << std::endl;

      //Initialize stop
      std::cout << "; initialize stop" << std::endl;
      std::cout << "\tmov qword [param_" << it->second.getFunctionName() << "+16], stop";
      std::cout << std::endl;
   }

   //Initialize mutex
   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\tmov rdi, write_mutex_" << it->second.getFunctionName()
                << std::endl;
      
      std::cout << "\tmov rsi, 0" << std::endl;
      std::cout << "call pthread_mutex_init" << std::endl;
   }

   for(auto it = connections.begin(); it != connections.end(); ++it) {
      //Initialize write_head_count
      std::cout << "; initialize write_head_count" << std::endl;
      std::cout << "\tmov r8d, " << std::to_string(it->second.size()) << std::endl;
      std::cout << "\tmov [param_" << definitions[it->first].getFunctionName()
         << "+32], r8d" << std::endl; 

      //Initialize write_heads
      std::cout << "\tmov rdi, " << std::to_string(it->second.size() * 16) << std::endl;
      std::cout << "\tcall malloc" << std::endl;
      std::cout << "\tmov [param_" << definitions[it->first].getFunctionName()
         << "+24], rax" << std::endl;

      for(auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
         std::cout << "\tmov qword [rax], write_head_" 
                   << definitions[*it2].getFunctionName() << std::endl;
         
         std::cout << "\tadd rax, 8" << std::endl;

         std::cout << "\tmov qword [rax], write_mutex_"
                   << definitions[*it2].getFunctionName()
                   << std::endl;
         std::cout << "\tadd rax, 8" << std::endl;
      }

      std::cout << std::endl;
   }


   //Generate pthreads
   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\txor rax, rax" << std::endl;

      std::cout << "\t" << "mov rdi, pthread_" << it->second.getFunctionName() 
         << std::endl;

      std::cout << "\tmov rsi, 0" << std::endl;
      std::cout << "\tmov rdx, " << it->second.getFunctionName() << std::endl;
      std::cout << "\tmov rcx, param_" << it->second.getFunctionName() << std::endl;
      std::cout << "\tcall pthread_create" << std::endl << std::endl;
   }

   for(auto it = definitions.begin(); it != definitions.end(); ++it) {
      std::cout << "\tmov rdi, [pthread_" 
                << it->second.getFunctionName() << "]" << std::endl;
      std::cout << "\tmov rsi, 0" << std::endl;
      std::cout << "\tcall pthread_join" << std::endl;
      
   }

   std::cout << "\tpop r12" << std::endl;

   std::cout << "\tmov rax, 60" << std::endl;
   std::cout << "\tmov rdi, 0" << std::endl;
   std::cout << "\tsyscall" << std::endl;
}

int main(int argc, char* argv[]) {
   input.open("pipeline");

   if (read_program())
      generate_asm();

   
}
