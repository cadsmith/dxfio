import { Analytics } from '@vercel/analytics/react'
import { StrictMode } from 'react'
import { createRoot, hydrateRoot } from 'react-dom/client'
import { App } from './App'
import { parse } from './route'
import './index.css'
import 'markdown-it-github-alerts/styles/github-colors-light.css'
import 'markdown-it-github-alerts/styles/github-colors-dark-class.css'
import 'markdown-it-github-alerts/styles/github-base.css'
import './styles.css'

const root = document.getElementById('root') as HTMLElement
const at = parse(window.location.pathname)
const app = (
  <StrictMode>
    <App at={at} />
    <Analytics />
  </StrictMode>
)

// A prerendered page is taken over as it is, once its HTML has painted: the frame after the first,
// so the app's work and the code it loads never hold the first paint back. Any other address is
// drawn anew at once.
if (root.dataset.path === at.join('/')) requestAnimationFrame(() => setTimeout(() => hydrateRoot(root, app)))
else createRoot(root).render(app)
