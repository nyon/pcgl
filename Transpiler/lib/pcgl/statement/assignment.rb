module PCGL
  module Statement
    # Assignment stub
    class Assignment < Base
      def self.priority
        0
      end

      def self.can_parse?(_line)
        false
      end
    end
  end
end
