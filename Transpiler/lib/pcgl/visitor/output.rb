module PCGL
  module Visitor
    # C++ output visitor
    class Output < Base
      attr_accessor :h_file
      attr_accessor :cpp_file

      def initialize(rules)
        @first_rule = true
        @rules = rules
        @class_scope = nil
        @h_file = ''
        @cpp_file = ''
      end

      def h_output(str)
        @h_file += str + "\n"
      end

      def cpp_output(str)
        @cpp_file += str + "\n"
      end

      def pre_visit_root(object)
        @class_scope = object.meta[:name]
        guard_name = "__#{object.meta[:name].to_snakecase.upcase}_H"
        h_output <<-CODE
#ifndef #{guard_name}
#define #{guard_name}

#include "BaseModel.h"
#include <functional>

using namespace std;
using namespace std::placeholders;

class #{object.meta[:name]} : public BaseModel
{
public:
CODE

        cpp_output "#include \"#{object.meta[:name]}.h\""
      end

      def pre_visit_if(object)
        h_output "    if(#{object.meta[:condition]}) {"
      end

      def post_visit_if(_object)
        h_output '    }'
      end

      def pre_visit_else_if(object)
        h_output "    else if(#{object.meta[:condition]}) {"
      end

      def post_visit_else_if(_object)
        h_output '    }'
      end

      def pre_visit_else(_object)
        h_output '    else {'
      end

      def post_visit_else(_object)
        h_output '    }'
      end

      def pre_visit_implicit_rule(object)
        h_output "  float #{object.meta[:name]}(float x, float y, float z) {"
      end

      def post_visit_implicit_rule(_object)
        h_output '  }'
        h_output '  '
      end

      def pre_visit_simple_rule(object)
        output_first_rule(object) if @first_rule && !object.meta[:implicit]
        h_output "  void #{object.meta[:name]}() {"
      end

      def post_visit_simple_rule(_object)
        h_output '  }'
        h_output '  '
      end

      def post_visit_raw(object)
        h_output "  #{object.line};"
      end

      def output_first_rule(object)
        h_output '  void __init() {'
        h_output "    #{object.meta[:name]}();"
        h_output '  }'
        @first_rule = false
      end

      def visit_command(object)
        parameter = object.meta[:parameter] || []
        parameter.map! do |e|
          apply_syntactic_sugar(e)
        end
        parameter.map! do |e|
          transform_parameter e
        end
        h_output "      #{object.meta[:name]}(#{parameter.join(',')});"
      end

      def transform_parameter(parameter)
        return parameter unless @rules.key? parameter
        if @rules[parameter].first.meta[:implicit]
          "bind(&#{@class_scope}::#{parameter}, this, _1, _2, _3)"
        else
          "bind(&#{@class_scope}::#{parameter}, this)"
        end
      end

      def visit_assignment(object)
        left = object.meta[:left]
        right = apply_syntactic_sugar(object.meta[:right] || '0')
        h_output "      auto #{left} = #{right};"
      end

      def post_visit_root(_object)
        h_output '};'
        h_output ''
        h_output '#endif'
      end
    end
  end
end
