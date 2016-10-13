module PCGL
  module Statement
    # rule definition stub
    class RuleDefinition < Base
      def self.priority
        0
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
