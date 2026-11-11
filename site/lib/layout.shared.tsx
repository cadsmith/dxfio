import type { BaseLayoutProps } from 'fumadocs-ui/layouts/shared'
import { Mark } from '@/components/mark'
import { appName, repository } from './shared'

export function baseOptions(): BaseLayoutProps {
  return {
    nav: {
      title: (
        <>
          <Mark size={24} />
          <span className="font-semibold">{appName}</span>
        </>
      ),
    },
    githubUrl: repository,
  }
}

// The home page's nav; in the docs the sidebar lists the same sections.
export const homeLinks: BaseLayoutProps['links'] = [
  { text: 'Guide', url: '/guide/start/' },
  { text: 'Tutorials', url: '/tutorials/reading/' },
  { text: 'Concepts', url: '/concepts/document/' },
  { text: 'Reference', url: '/reference/' },
]
