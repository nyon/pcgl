require_relative './base'

module PCGL
  module Visitor
    # rule collector, collects all rules and
    # saves them in an array
    class RuleCollector < Base
      attr_accessor :rules

      def initialize
        @rules = {}
      end

      def visit_conditional_rule(object)
        visit_rule(object)
      end

      def visit_simple_rule(object)
        visit_rule(object)
      end

      def visit_implicit_rule(object)
        visit_rule(object)
      end

      def visit_rule(object)
        @rules[object.meta[:name]] ||= []
        @rules[object.meta[:name]].push(object)
      end
    end
  end
end
