module PCGL
  module Visitor
    # translates rule definitions into an explicit
    # or implicit rule
    class RuleDefinitionTransformer < Base
      attr_accessor :rules

      def initialize(rules)
        @rules = rules
      end

      def transform_rule(object)
        name = object.meta[:name]
        return Statement::Empty.new if @rules[name].nil?

        st = Statement::RuleDefinition.new
        st.meta[:name] = name
        st.meta[:implicit] = object.meta[:implicit]
        st.children = @rules[name]
        @rules.delete(name)
        st
      end
    end
  end
end
