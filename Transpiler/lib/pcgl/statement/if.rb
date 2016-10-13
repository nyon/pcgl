module PCGL
  module Statement
    # if stub
    class If < Base
      def self.priority
        0
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
