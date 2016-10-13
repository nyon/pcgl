module PCGL
  module Statement
    # elseif stub
    class ElseIf < Base
      def self.priority
        0
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
